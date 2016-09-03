#include <scheduler.h>

void scheduler(){
	/* There is a running process */
	if (curProc){
		curProc->global_time += getTODLO() - procInit;

		/* Set process start time in the CPU
		curProc->p_cpu_time += getTODLO() - ProcessTOD ;
		ProcessTOD  = getTODLO();

		/* Update elapsed time of the Pseudo-Clock tick
		TimerTick  += getTODLO() - StartTimerTick;
		StartTimerTick = getTODLO();

		/* Set Interval Timer as the smallest between Time Slice and Pseudo-Clock tick
		setTIMER(MIN((SCHED_TIME_SLICE - curProc->p_cpu_time), (SCHED_PSEUDO_CLOCK - TimerTick )));

		/* Load the processor state in order to start execution*/
		LDST(&(curProc->p_s));
	}
	/* [Case 2] There is not a running process*/
	else{
		/* If Ready Queue is empty*/
		if (clist_empty(readyQueue)){
			/* no more processes*/
			if (processCounter == 0) HALT();
			/* Deadlock Detection*/
			if (processCounter > 0 && softBlockCounter == 0) PANIC();
			/* wait for interrupts */
			if (processCounter > 0 && softBlockCounter > 0){
				/* Enable interrupts*/
				setSTATUS(STATUS_ALL_INT_ENABLE(getSTATUS()));
				WAIT();
			}
			PANIC(); /* Anomaly*/
		}

		/* Otherwise extract first ready process*/
		if (!(curProc = removeProcQ(&readyQueue))) PANIC(); /* Anomaly */

		/* Compute elapsed time from the Pseudo-Clock tick
		TimerTick  += getTODLO() - StartTimerTick;
		StartTimerTick = getTODLO();

		/* Initialize global time */
		procInit = getTODLO();

		/* Set Interval Timer as the smallest between Time Slice and Pseudo-Clock tick
		setTIMER(MIN(SCHED_TIME_SLICE, (SCHED_PSEUDO_CLOCK - TimerTick )));

		/* Load the processor state in order to start execution*/
		LDST(&(curProc->p_s));
	}
}


/*void scheduler(sched_t status) {
	if (status != SCHED_START && status != SCHED_NEXT && status != SCHED_RESET && status != SCHED_CONTINUE)
		PANIC();

    if (status == SCHED_START) {
    	setTIMER(SCHED_TIME_SLICE);		// dico ad uARM che il time slice è di 5ms
    }
    // Manca la gestione dei tempi
    if (status != SCHED_CONTINUE) {		// codice eseguito sempre, tranne nel caso in cui lo scheduler deve ricaricare il processo interrotto
	    if (curProc) {
	    	insertProcQ(&readyQueue, curProc);
	    	curProc = NULL;
	    }

	    curProc = removeProcQ(&readyQueue);
	    if (!curProc) {
	    	if (clist_empty(readyQueue)) {//!readyQueue
	    		if (processCounter == 0) HALT();
	    		if (processCounter > 0 && softBlockCounter == 0) PANIC();
	    		if (processCounter > 0 && softBlockCounter > 0) {
						setSTATUS(STATUS_ALL_INT_ENABLE(getSTATUS()));
						WAIT();
					}
	    	}
	    }
	}
	//(curProc->p_s).cpsr = STATUS_ALL_INT_ENABLE((curProc->p_s).cpsr); // davvero necessario?
	LDST(&(curProc->p_s));
}*/
