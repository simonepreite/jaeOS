#ifndef SYSCALL_H
#define SYSCALL_H

#include <initial.h>

EXTERN void saveCurState(state_t *state, state_t *newState);
EXTERN int createProcess(state_t *stato);
EXTERN void terminateProcess(int p);
EXTERN void semaphoreOperation(int *sem, int weight);
EXTERN pid_t getPid();

#endif
