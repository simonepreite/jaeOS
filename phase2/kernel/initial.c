#include <uARMconst.h>
#include <uARMtypes.h>
#include "../../phase1/include/types.h"

UI processCounter;
UI softBlockCounter;
pcb_t *readyQueue;
pcb_t *curProc;

void initArea(memaddr area, memaddr handler){
    state_t *newArea = (state_t*) area;
    /* Memorizza il contenuto attuale del processore in newArea */
    STST(newArea);
    /* Setta pc alla funzione che gestirà l'eccezione */
    newArea->pc = handler;
    /* Setta sp a RAMTOP */
    newArea->sp = RAMTOP;
    /* Setta il registro di Stato per mascherare tutti gli interrupt e si mette in kernel-mode. */
    newArea->cpsr=STATUS_ALL_INT_DISABLE((newArea->cpsr) | STATUS_SYS_MODE);
    /* Disabilita la memoria virtuale */
        /* setCONTROL((getCONTROL()) & ~(DISABLE_VM));*/
    newArea->CP15_Control = (newArea->CP15_Control) & ~(ENABLE_VM);
}


int main(){

	pcb_t *firstProcess;

	processCount = 0;

	softBlockCounter = 0;

    curProc = NULL;

		initArea(INT_NEWAREA, (memaddr)intHandler);
		initArea(TLB_NEWAREA, (memaddr)tlbHandler);
		initArea(PGMTRAP_NEWAREA, (memaddr)pgmHandler);
		initArea(SYSBK_NEWAREA, (memaddr)sysBpHandler);

		initPcbs();

    initASL();

    firstProcess = allocPcb();
    if(firstProcess == NULL){
        PANIC();
    }

    insertProcQ(&readyQueue, firstProcess);

    processCounter++;

    scheduler();

	return 0;
}
