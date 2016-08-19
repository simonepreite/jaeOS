#include <initial.h>

struct clist readyQueue = CLIST_INIT;

void initArea(memaddr area, memaddr handler){
    state_t* newArea = (state_t*)area;
    // Store current CPU state in newArea
    STST(newArea);
    // Set program counter to handler address
    newArea->pc = handler;
    // Set stack pointer to RAMTOP
    newArea->sp = RAM_TOP;
    // Set state register to mask interrupts and execute in kernel mode
    newArea->cpsr = STATUS_NULL | STATUS_SYS_MODE;
    newArea->cpsr = STATUS_ALL_INT_DISABLE(newArea->cpsr);
}


int main(){
  int i;
	pcb_t *firstProcess;

    // Initialize kernel variables
	processCounter = 0;
	softBlockCounter = 0;
  curProc = NULL;

    print("main");

    // Initialize new areas in ROM Reserved Frame
    initArea(INT_NEWAREA, (memaddr)intHandler);
	  initArea(TLB_NEWAREA, (memaddr)tlbHandler);
	  initArea(PGMTRAP_NEWAREA, (memaddr)pgmHandler);
	  initArea(SYSBK_NEWAREA, (memaddr)sysHandler);

    print("init area OK");

    // Initialize Phase1 data structures
    initPcbs();
    initASL();

    print("init OK");

    // Initialize device semaphores to 0
    for(i = 0; i < MAX_DEVICES; i++)
        semDevices[i] = 0;

    // Try to alloc first process
    if((firstProcess = allocPcb()) == NULL)
        PANIC();

    firstProcess->pid = genPid();   // Generate Process ID
    firstProcess->p_s.cpsr = STATUS_NULL;
    firstProcess->p_s.cpsr = firstProcess->p_s.cpsr | STATUS_SYS_MODE;  // Process executed in kernel mode
    firstProcess->p_s.CP15_Control = CP15_DISABLE_VM(firstProcess->p_s.CP15_Control);   // Virtual memory disabled
    firstProcess->p_s.sp = RAM_TOP - FRAME_SIZE; // Setting stack pointer
    firstProcess->p_s.pc = (memaddr)test;   // Setting program counter

    // Process put in ready queue
    insertProcQ(&readyQueue, firstProcess);
    processCounter++;
    scheduler(SCHED_START);

	return 0;
}
