#ifndef EXCEPITION_H
#define EXCEPITION_H

#include <const.h>
#include <initial.h>

EXTERN cputime_t kernelStart;
int debug_a1; //debug variable

// auxiliary prototype

EXTERN void saveCurState(state_t *state, state_t *newState);
void handlerSYSTLBPGM(UI old, UI new, state_t* state);

// handler prototype

void tlbHandler();
void sysHandler();
void pgmHandler();

// SYSTEM CALL

EXTERN int createProcess(state_t *stato);
EXTERN void terminateProcess(pid_t p);
EXTERN void semaphoreOperation(int *sem, int weight);
EXTERN void specifySysBp(memaddr handler, memaddr stack, UI flags);
EXTERN void specifyTlb(memaddr handler, memaddr stack, UI flags);
EXTERN void specifyPgm(memaddr handler, memaddr stack, UI flags);
EXTERN void exitTrap(UI exType, UI ret);
EXTERN void getCpuTime(cputime_t *global_time, cputime_t *user_time);
EXTERN void waitForClock();
EXTERN UI iodevop(UI command, int lineNum, UI deviceNum);
EXTERN pid_t getPid();

#endif
