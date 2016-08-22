#ifndef SYSCALL_H
#define SYSCALL_H

#include <initial.h>

void saveCurState(state_t *state, state_t *newState);
int createProcess(state_t *stato);
pcb_t* searchPid(pcb_t *parent, pid_t pid, pcb_t* save);
void terminateProcess(pid_t p);
void semaphoreOperation(int *sem, int weight);
UI iodevop(UI command, int lineNum, UI deviceNum);
void getCpuTime(cputime_t *global_time, cputime_t *user_time);
pid_t getPid();

#endif
