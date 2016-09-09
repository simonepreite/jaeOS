#ifndef INITIAL_H
#define INITIAL_H

#include <uARMconst.h>
#include <uARMtypes.h>
#include <arch.h>
#include <pcb.h>
#include <asl.h>
#include <exceptions.h>
#include <interrupts.h>
#include <libuarm.h>

// process needed

UI processCounter;		// number of total processes
UI softBlockCounter;	// number of processes waiting for an interrupt
EXTERN struct clist readyQueue;
int semDevices[MAX_DEVICES];
pcb_t *curProc;

// timing

cputime_t kernelStart;
cputime_t clock;
cputime_t clockTick;
cputime_t processStart;


// auxiliary function

void initArea(memaddr area, memaddr handler);

// extern modules function

EXTERN void test();
EXTERN void scheduler();
EXTERN pid_t genPid(UI a);


#endif
