#ifndef INITIAL_H
#define INITIAL_H

#include <uARMconst.h>
#include <uARMtypes.h>
#include <arch.h>
#include <pcb.h>
#include <asl.h>
#include <exceptions.h>
#include <interrupts.h>
#include <scheduler.h>
#include <syscall.h>
#include <libuarm.h>

UI processCounter;		// number of total processes
UI softBlockCounter;	// number of processes waiting for an interrupt
struct clist *readyQueue;
int semDevices[MAX_DEVICES];
pcb_t *curProc;

EXTERN void test();
EXTERN void print(char *msg);
EXTERN void initArea(memaddr area, memaddr handler);


#endif
