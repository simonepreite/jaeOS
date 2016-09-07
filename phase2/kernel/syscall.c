#include <syscall.h>
EXTERN synp4;
/***************************************************************
*                      AUXILIARY FUNCTION                      *
***************************************************************/

// c'è la STST che fa la stessa cosa, forse!

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
	count = getTODLO();  //pseudo random generator
	count &= 0x0000FFFF;
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
			if(!emptyChild(scan)) {
      			tprint("recursion searchPid\n");
				save = searchPid(headProcQ(&scan->p_children), pid);
			}
			if(save)
				return save;
		}
	}
 	return NULL;
}

void terminator(pcb_t* proc) {
	while(!emptyChild(proc)) {
		testfun();
		terminator(removeChild(proc));
  	}
	if (proc->p_cursem!=NULL) {
		if ((int*)(proc->p_cursem) < &semDevices[0] || (int*)(proc->p_cursem) > &semDevices[MAXPROC-1]) {
			updateSemaphoreValue(proc->p_cursem, -proc->waitingResCount);
			outBlocked(proc);
			softBlockCounter--;
		}
	}
	processCounter--;
	if (proc != curProc){
    	//outChild(proc);
			freePcb(proc);
	}
}

// azioni ripetitive syscall 4,5,6

void setSYSTLBPGMT(UI old, UI new, memaddr handler, memaddr stack, UI flags){
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

	STST(&(curProc->excp_state_vector[old]));

	saveCurState(&curProc->p_s, &curProc->excp_state_vector[new]);

	curProc->excp_state_vector[new].pc = handler;
	curProc->excp_state_vector[new].sp = stack;
	curProc->p_s.cpsr &= STATUS_CLEAR_MODE;
	(curProc->p_s).cpsr = STATUS_ALL_INT_ENABLE((curProc->p_s).cpsr);
	curProc->excp_state_vector[new].cpsr |= flags;
	curProc->excp_state_vector[new].CP15_Control = CP15_ENABLE_VM(curProc->excp_state_vector[new].CP15_Control);
	curProc->excp_state_vector[new].CP15_EntryHi = setEntryHi(getEntryHi());
}

/***************************************************************
*                     SYSCALL KERNEL MODE                      *
***************************************************************/

int createProcess(state_t *stato){
	pcb_t *newProc = allocPcb();

	if(newProc == NULL) return -1; // fail

	saveCurState(stato, &(newProc->p_s));

	processCounter++;

	insertChild(curProc, newProc);
	newProc->pid = genPid(newProc->pid);
	insertProcQ(&readyQueue, newProc);

	return newProc->pid; // Success
}

void terminateProcess(pid_t p){
	pcb_t *save = NULL;

	if(p == 0 || curProc->pid == p){
		terminator(curProc);
		//outChild(curProc);
		removeChild(curProc);
		//outProcQ(&readyQueue, curProc);
		freePcb(curProc);
    	curProc=NULL;
	}
	else{
		if(!(save=searchPid(curProc, p))){
			PANIC();
		}
		terminator(save);
	}
}

void semaphoreOperation(int *sem, int weight){
	if (!sem) PANIC();
	if(weight >= 1){	// resources to be freed
		(*sem) += weight;

		pcb_t *firstBlocked;// = headBlocked(sem);

		while ((firstBlocked=headBlocked(sem)) && firstBlocked->waitingResCount <= weight) {
			firstBlocked = outBlocked(firstBlocked);		// rimuovo il processo dalla coda del semaforo
			softBlockCounter--;								// decremento il contatore dei processi bloccati sof
			weight -= firstBlocked->waitingResCount;
			firstBlocked->waitingResCount = 0;
			insertProcQ(&readyQueue, firstBlocked);
		}
		firstBlocked->waitingResCount -= weight; //guardare bene il discorso pesi potrebbe non funzionare sempre
	}
	else if (weight <= -1){		// resources to be allocated
		//if(sem == &synp4) testfun();
		(*sem) += weight;
		if(*sem < 0){
			curProc->waitingResCount = -weight;		// il processo ha bisogno di weight risorse

			 curProc->kernel_mode += getTODLO() - kernelStart;
			 curProc->global_time += getTODLO() - processStart;

			if(insertBlocked(sem, curProc))
				PANIC();

			softBlockCounter++;
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
	//curProc->global_time += getTODLO() - procInit;
	//curProc->kernel_mode += getTODLO() - kernelStart;

	//curProc->global_time += getTODLO() - processStart;
	//processStart = getTODLO();

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
