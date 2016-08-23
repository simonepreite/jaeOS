#ifndef EXCEPITION_H
#define EXCEPITION_H

#include <const.h>
#include <initial.h>


EXTERN cputime_t kernelStart;
void pgmHandler();
void tlbHandler();
void sysHandler();
EXTERN void saveCurState(state_t *state, state_t *newState);
EXTERN int createProcess(state_t *stato);
EXTERN void terminateProcess(pid_t p);
EXTERN void semaphoreOperation(int *sem, int weight);
EXTERN UI iodevop(UI command, int lineNum, UI deviceNum);
EXTERN void exitTrap(UI exType, UI ret);
EXTERN void getCpuTime(cputime_t *global_time, cputime_t *user_time);
EXTERN pid_t getPid();

#endif
