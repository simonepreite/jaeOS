#ifndef SYSCALL_H
#define SYSCALL_H

#include <initial.h>

void saveCurState(state_t *state, state_t *newState);
int createProcess(state_t *stato);
void terminateProcess(pid_t p);
void semaphoreOperation(int *sem, int weight);
pid_t getPid();

#endif
