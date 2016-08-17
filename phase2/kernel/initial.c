#include <initial.h>

void initArea(memaddr area, memaddr handler){
    state_t* newArea = (state_t*)area;
    /* Memorizza il contenuto attuale del processore in newArea */
    STST(newArea);
    /* Setta pc alla funzione che gestirà l'eccezione */
    newArea->pc = handler;
    /* Setta sp a RAMTOP */
    newArea->sp = RAM_TOP;
    /* Setta il registro di Stato per mascherare tutti gli interrupt e si mette in kernel-mode. */
    newArea->cpsr = STATUS_NULL  | STATUS_SYS_MODE;
    newArea->cpsr=STATUS_ALL_INT_DISABLE(newArea->cpsr);
}


int main(){
  int i;
	pcb_t *firstProcess;
	processCounter = 0;
	softBlockCounter = 0;
  curProc = NULL;
  readyQueue = NULL;
    print("main");
		initArea(INT_NEWAREA, (memaddr)intHandler);
		initArea(TLB_NEWAREA, (memaddr)tlbHandler);
		initArea(PGMTRAP_NEWAREA, (memaddr)pgmHandler);
		initArea(SYSBK_NEWAREA, (memaddr)sysHandler);
    print("init area OK");
		initPcbs();
    initASL();
    print("init OK");
    for(i = 0; i < MAX_DEVICES; i++) semDevices[i] = 0;

    if((firstProcess = allocPcb())==NULL){
        PANIC();
    }
    firstProcess->pid = genPid();
    firstProcess->p_s.cpsr = STATUS_NULL;
    firstProcess->p_s.cpsr = firstProcess->p_s.cpsr | STATUS_SYS_MODE;
    firstProcess->p_s.CP15_Control = CP15_DISABLE_VM(firstProcess->p_s.CP15_Control);
    firstProcess->p_s.sp = RAM_TOP - FRAME_SIZE;
    firstProcess->p_s.pc = (memaddr)test;

    insertProcQ(readyQueue, firstProcess);
    processCounter++;
    scheduler();

	return 0;
}
