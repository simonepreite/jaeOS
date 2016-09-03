#ifndef INITIAL_H
#define INITIAL_H

#include <uARMconst.h>
#include <uARMtypes.h>
#include <arch.h>
#include <pcb.h> //contiene types.h
#include <asl.h>
#include <exceptions.h>
#include <interrupts.h>
#include <libuarm.h>

UI processCounter;		// number of total processes
UI softBlockCounter;	// number of processes waiting for an interrupt
EXTERN struct clist readyQueue;
int semDevices[MAX_DEVICES];
pcb_t *curProc;
cputime_t kernelStart;

EXTERN void test();
EXTERN void scheduler();
EXTERN pid_t genPid(UI a);
void initArea(memaddr area, memaddr handler);


#endif
