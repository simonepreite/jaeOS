/*
 *	SCHEDULER.C
 *	Scheduling Policy and Deadlock Detection Implementation File
 *
 *	Gruppo 28:
 *	Del Vecchio Matteo
 *	Preite Simone
 */

#include <scheduler.h>

//cputime_t executed = 0;

void scheduler() {

	if (!curProc) {
		if (clist_empty(readyQueue)) {
			/* no more processes*/
			if (processCounter == 0) HALT();
			/* Deadlock Detection*/
			if (processCounter > 0 && softBlockCounter == 0) PANIC();
			/* wait for interrupts */
			if (processCounter > 0 && softBlockCounter > 0) {
				setSTATUS(STATUS_ALL_INT_ENABLE(getSTATUS()));
				WAIT();
			}
			PANIC(); /* Anomaly*/
		}

		if (!(curProc = removeProcQ(&readyQueue))) PANIC(); /* Anomaly */

		clock += getTODLO() - clockTick;
		clockTick = getTODLO();
		processStart = getTODLO();

		if(curProc->remaining < 5000) setTIMER(curProc->remaining);
		else setTIMER(SCHED_TIME_SLICE);
	}
	else {
		curProc->global_time += getTODLO() - processStart;

		clock += getTODLO() - clockTick;
		clockTick = getTODLO();

		setTIMER(SCHED_TIME_SLICE - (getTODLO() - processStart));
		processStart = getTODLO();
	}

	LDST(&(curProc->p_s));
}
