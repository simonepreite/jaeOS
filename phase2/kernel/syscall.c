#include <syscall.h>


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

/***************************************************************
*                     SYSCALL KERNEL MODE                      *
***************************************************************/

// questa funziona si occupa di assegnare pid univoci ai processi

pid_t genPid(unsigned int a){
	HIDDEN unsigned int count = 0;

	count++;
	return a + count;
}

int createProcess(state_t *stato){
	pcb_t *newProc = allocPcb();

	if(newProc == NULL) return -1; // fail

	saveCurState(stato, &(newProc->p_s));

	processCounter++;

	insertChild(curProc, newProc);
	newProc->pid = genPid(newProc->pid);
	insertProcQ(&readyQueue, newProc);

	return 0; // Success
}

void searchPid(pcb_t *parent, pid_t pid, pcb_t* save){
	void* tmp = NULL;
	pcb_t* scan;
	clist_foreach(scan, &(parent->p_children), p_siblings, tmp){
		if(scan->pid==pid){
			save=scan;
			break;
		}
		else {
			if(!emptyChild(scan))
				searchPid(headProcQ(&scan->p_children), pid, save);
			if(save)
				break;
		}
	}
}

void terminator(pcb_t* proc){
	while(!emptyChild(proc))
		terminator(removeChild(proc));
	//controllare se il processo è bloccato ad un semaforo
	//altrimenti toglierlo dalla lista dei processi pronti
	processCounter--;
	if (proc != curProc)
		freePcb(proc);
}

void terminateProcess(pid_t p){
	pcb_t* save = NULL;

	if(p == 0 || curProc->pid == p){
		terminator(curProc);
		outChild(curProc);
		outProcQ(&readyQueue, curProc);
		freePcb(curProc);
		// bisogna dire allo scheduler di caricare il processo successivo
		//scheduler(SCHED_RUNNING);		// approfondire se si può fare
	}
	else{
		searchPid(curProc, p, save);
		if(!save) PANIC();
		terminator(save);
		// lo scheduler deve ricaricare curProc
		//scheduler(SCHED_CONTINUE);
	}
}

void semaphoreOperation(int *sem, int weight){
	if(weight == 1){
		pcb_t *firstBlock;
		(*sem)++;
		firstBlock = removeBlocked(sem);

		if(firstBlock != NULL){
			insertProcQ(&readyQueue, firstBlock);
			//decremento di softBlockCounter se sono su un semaforo soft
			firstBlock->p_cursem = NULL;
		}
	}
	else if (weight == -1){
		(*sem)--;
		if(*sem < 0){
			//inserire kernel time su nuovi elementi della struttura
			if(insertBlocked(sem, curProc))
				PANIC();
			//controllare incremento di softBlockCounter solo su semafori soft
			//dire allo scheduler di caricare il processo successivo
			curProc = NULL;
		}
	}
	else if (weight == 0){
		terminateProcess(curProc->pid);
	}
	else PANIC();
}

void getCpuTime(cputime_t *global_time, cputime_t *user_time){
	*global_time = curProc->global_time;
	*user_time = curProc->global_time - curProc->kernel_mode;
}

pid_t getPid(){
	return curProc->pid;
}
