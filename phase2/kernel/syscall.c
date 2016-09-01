#include <syscall.h>

/***************************************************************
*                      AUXILIARY FUNCTION                      *
***************************************************************/

// c'è la STST che fa la stessa cosa

void saveCurState(state_t *state, state_t *newState){
	newState->a1 = state->a1;
	newState->a2 = state->a2;
	newState->a3 = state->a3;
	newState->a4 = state->a4;
	newState->v1 = state->v1;
	newState->v2 = state->v2;
	newState->v3 = state->v3;
	newState->v4 = state->v4;
	newState->v5 = state->v5;
	newState->v6 = state->v6;
	newState->sl = state->sl;
	newState->fp = state->fp;
	newState->ip = state->ip;
	newState->sp = state->sp;
	newState->lr = state->lr;
	newState->pc = state->pc;
	newState->cpsr = state->cpsr;
	newState->CP15_Control = state->CP15_Control;
	newState->CP15_EntryHi = state->CP15_EntryHi;
	newState->CP15_Cause = state->CP15_Cause;
	newState->TOD_Hi = state->TOD_Hi;
	newState->TOD_Low = state->TOD_Low;
}

// questa funziona si occupa di assegnare pid univoci ai processi

pid_t genPid(UI a){
	HIDDEN UI count = 0;

	count++;
	return a + count;
}

// supporto alla terminate process

pcb_t* searchPid(pcb_t *parent, pid_t pid){
	void* tmp = NULL;
	pcb_t* scan;
  pcb_t* save = NULL;
	clist_foreach(scan, &(parent->p_children), p_siblings, tmp){
		if(scan->pid==pid){
      tprint("if searchPid\n");
			return scan;
		}
		else {
			if(!emptyChild(scan))
      tprint("recursion searchPid\n");
				save = searchPid(headProcQ(&scan->p_children), pid);
			if(save)
				return save;
		}
	}
  return NULL;
}

void terminator(pcb_t* proc){
  tprint("terminator\n");
	while(!emptyChild(proc)){
    tprint("recursion\n");
		terminator(removeChild(proc));
  }
	//controllare se il processo è bloccato ad un semaforo
	//altrimenti toglierlo dalla lista dei processi pronti
	processCounter--;
	if (proc != curProc)
    outChild(proc);
		freePcb(proc);
}

// azioni ripetitive syscall 4,5,6

void setSYSTLBPGMT(UI old, UI new, memaddr handler, memaddr stack, UI flags){

	/* domanda aperta: come fare senza specificare un nuovo array nella struttura dati?
	 idea utilizzo la maschera di bit che usa similmente a linux per rwx permettendo così
	 di utilizzare un solo intero invece di un array */

	if (old == SYS) {
		switch (curProc->tags) {
			case 1:
			case 3:
			case 5:
			case 7:
				terminateProcess(0);
				break;
			default:
				curProc->tags ^= 1;
				break;
		}
	}
	else if (old == TLB) {
		switch (curProc->tags) {
			case 2:
			case 3:
			case 6:
			case 7:
				terminateProcess(0);
				break;
			default:
				curProc->tags ^= 2;
				break;
		}
	}
	else if (old == PGMT) {
		switch (curProc->tags) {
			case 4:
			case 5:
			case 6:
			case 7:
				terminateProcess(0);
				break;
			default:
				curProc->tags ^= 4;
				break;
		}
	}
	else PANIC();

	//STST(&(curProc->excp_state_vector[old])); // old o new?

	saveCurState(&curProc->p_s, &curProc->excp_state_vector[new]); // istruzione probabilmente molto inutile, che differenza c'è tra questa e STST?

	curProc->excp_state_vector[new].pc = handler;
	curProc->excp_state_vector[new].sp = stack;
	flags &= (0x80000007); // 3 bit meno significativi a 1 per settare VM
	curProc->excp_state_vector[new].cpsr &= (0x7FFFFFF8); // 3 bit più significativi essendo 7 non sono sicuro della faccenda, APPROFONDIRE
	curProc->excp_state_vector[new].cpsr |= flags;
	curProc->excp_state_vector[new].CP15_EntryHi = setEntryHi(getEntryHi());
}

/***************************************************************
*                     SYSCALL KERNEL MODE                      *
***************************************************************/

int createProcess(state_t *stato){
	pcb_t *newProc = allocPcb();

	if(newProc == NULL) return -1; // fail

	saveCurState(stato, &(newProc->p_s));

	//STST(newProc);

	processCounter++;

	insertChild(curProc, newProc);
	newProc->pid = genPid(newProc->pid);
	insertProcQ(&readyQueue, newProc);

	return 0; // Success
}

void terminateProcess(pid_t p){
	pcb_t *save = NULL;

	if(p == 0 || curProc->pid == p){
		terminator(curProc);
		outChild(curProc);
		outProcQ(&readyQueue, curProc);
		freePcb(curProc);
    curProc=NULL;
		// bisogna dire allo scheduler di caricare il processo successivo
		//scheduler(SCHED_NEXT);		// approfondire se si può fare
	}
	else{
		if(!(save=searchPid(curProc, p))){
			PANIC();
		}
		terminator(save);
		// lo scheduler deve ricaricare curProc
		//scheduler(SCHED_CONTINUE);
	}
}

void semaphoreOperation(int *sem, int weight){
	if (!sem) PANIC();

	if(weight >= 1){	// resources to be freed
		(*sem) += weight;
		pcb_t *firstBlocked = headBlocked(sem);

		if(firstBlocked != NULL){		// c'è un processo bloccato sul semaforo
			if (firstBlocked->waitingResCount <= (*sem)) {		// si sono liberate abbastanza risorse per liberarlo
				firstBlocked = outBlocked(firstBlocked);		// rimuovo il processo dalla coda del semaforo
				softBlockCounter--;								// decremento il contatore dei processi bloccati soft
				firstBlocked->p_cursem = NULL;					// annullo il conllegamento tra il processo ed il semaforo
				firstBlocked->waitingResCount = 0;				// il processo non è più in attesa di risorse
				insertProcQ(&readyQueue, firstBlocked);			// inserisco il processo nella coda ready
			}
		}
	}
	else if (weight <= -1){		// resources to be allocated
		(*sem) += weight;
		if(*sem < 0){
			//inserire kernel time su nuovi elementi della struttura
			curProc->waitingResCount = weight;		// il processo ha bisogno di weight risorse

			if(insertBlocked(sem, curProc))
				PANIC();

			softBlockCounter++;
			//dire allo scheduler di caricare il processo successivo
			curProc = NULL;
		}
	}
	else
		terminateProcess(curProc->pid);
}

void specifySysBp(memaddr handler, memaddr stack, UI flags){
	setSYSTLBPGMT(SYS, EXCP_SYS_NEW, handler, stack, flags);
}

void specifyTlb(memaddr handler, memaddr stack, UI flags){
	setSYSTLBPGMT(TLB, EXCP_TLB_NEW, handler, stack, flags);
}

void specifyPgm(memaddr handler, memaddr stack, UI flags){
	setSYSTLBPGMT(PGMT, EXCP_PGMT_NEW, handler, stack, flags);
}

void exitTrap(UI exType, UI ret){

	switch(exType){
		case SYS:{
			curProc->excp_state_vector[EXCP_SYS_OLD].a1 = ret;
			LDST(&curProc->excp_state_vector[EXCP_SYS_OLD]);
			break;
		}
		case TLB:{
			curProc->excp_state_vector[EXCP_TLB_OLD].a1 = ret;
			LDST(&curProc->excp_state_vector[EXCP_TLB_OLD]);
			break;
		}
		case PGMT:{
			curProc->excp_state_vector[EXCP_PGMT_OLD].a1 = ret;
			LDST(&curProc->excp_state_vector[EXCP_PGMT_OLD]);
			break;
		}
		default:{
			PANIC();
		}
	}
}

void getCpuTime(cputime_t *global_time, cputime_t *user_time){
	*global_time = curProc->global_time;
	*user_time = curProc->global_time - curProc->kernel_mode;
}

void waitForClock(){
	softBlockCounter++;			// there's one more blocked process
	int index = MAX_DEVICES - 1;	// pseudo clock semaphore is the last one in the array
	semaphoreOperation(&semDevices[index], -1);		// lock the current semaphore
}

UI iodevop(UI command, int lineNum, UI deviceNum) {
	devreg_t *deviceRegister = (devreg_t *)DEV_REG_ADDR(lineNum, deviceNum);	// indirizzo al device register del dispositivo, sia esso terminale o altro

	UI terminalReading = (lineNum == INT_TERMINAL && deviceNum >> 31) ? N_DEV_PER_IL : 0;	// controllo se voglio ricevere o trasmettere dal terminale
	int index = EXT_IL_INDEX(lineNum) * N_DEV_PER_IL;		// calcolo l'indice del primo semaforo associato all'interrupt line richiesta
	index += deviceNum;			// calcolo l'indice del semaforo per il device richiesto
	index += terminalReading;		// aggiungo 8 o meno, in base al tipo di operazione sul terminale

	semaphoreOperation(&semDevices[index], -1);		// eseguo SYS3 con peso -1 perchè il processo deve bloccarsi e il peso può solo essere +/-1

	UI status = 0;
	// Riprendo l'esecuzione dopo l'interrupt
	if (lineNum == INT_TERMINAL) {
		if (terminalReading > 0) {		// terminal in lettura
			deviceRegister->term.recv_command = command;
			status = deviceRegister->term.recv_status;
		}
		else {		// terminal in scrittura
			deviceRegister->term.transm_command = command;
			status = deviceRegister->term.transm_status;
		}
	}
	else {		// altri dispositivi
		deviceRegister->dtp.command = command;
		status = deviceRegister->dtp.status;
	}
	return status;
}

pid_t getPid(){
	return curProc->pid;
}
