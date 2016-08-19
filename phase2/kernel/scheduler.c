#include <scheduler.h>

void scheduler(sched_t status) {
	if (status != SCHED_START && status != SCHED_RUNNING && status != SCHED_RESET && status != SCHED_CONTINUE)
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
	    		if (processCounter > 0 && softBlockCounter > 0) WAIT();
	    	}
	    }
	}
	(curProc->p_s).cpsr = STATUS_ALL_INT_ENABLE((curProc->p_s).cpsr); // davvero necessario?
  LDST(&(curProc->p_s));
}
