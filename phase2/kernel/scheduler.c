#include <initial.h>

void scheduler(){

    /* Se c'è un processo in esecuzione */


		/* Carica lo stato del processo corrente */
		LDST(&(curProc->p_s));

    /* gestione deadlock*/
    if(curProc == NULL) {

        /* Se la readyQueue è vuota */
        if(readyQueue==NULL) {
            /* Se processCount è zero chiamo HALT */
            if(processCounter==0){
                HALT();
            /* Se processCount > 0 e softBlockCount vale 0. Si è verificato un deadlock, invoco PANIC() */
          }else if(processCounter>0 && softBlockCounter==0) {
                PANIC();
            /* Se processCount>0 e softBlockCount>0 mi metto in stato d iattesa, invoco WAIT() */
          }else if(processCounter>0 && softBlockCounter>0) {
                WAIT();
            }
        }
        curProc = removeProcQ(readyQueue);

        /* caso anomalo */
        if(curProc == NULL){
             PANIC();
        }
        //setNextTimer();

		/* Carica lo stato del processo sul processore */
		LDST(&(curProc->p_s));
    }

}
