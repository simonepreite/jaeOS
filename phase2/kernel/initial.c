#include <uARMconst.h>
#include <uARMtypes.h>

UI processCounter;
UI softBlockCounter;
pcb_t *readyQueue;
pcb_t *curProc;




int main(){

	pcb_t *firstProcess;

	processCount = 0;

	softBlockCounter = 0;

    curProc = NULL;

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