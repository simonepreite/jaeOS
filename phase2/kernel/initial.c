#include <uARMconst.h>
#include <uARMtypes.h>

UI processCounter;
UI softBlockCounter;
pcb_t *readyQueue;
pcb_t *currentProcess;




int main(){

	pcb_t *firstProcess;

	initPcbs();

    initASL();

    scheduler();

	return 0;
}