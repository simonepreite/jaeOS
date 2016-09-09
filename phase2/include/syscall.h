#ifndef SYSCALL_H
#define SYSCALL_H

#include <initial.h>

// auxiliary prototype

void saveCurState(state_t *state, state_t *newState);
pid_t genPid(UI a);
pcb_t* searchPid(pcb_t *parent, pid_t pid);
void terminator(pcb_t* proc);
void setSYSTLBPGMT(hdl_type old, UI new, memaddr handler, memaddr stack, UI flags);

//  system call prototype

int createProcess(state_t *stato);
void terminateProcess(pid_t p);
void semaphoreOperation(int *sem, int weight);
void specifySysBp(memaddr handler, memaddr stack, UI flags);
void specifyTlb(memaddr handler, memaddr stack, UI flags);
void specifyPgm(memaddr handler, memaddr stack, UI flags);
void exitTrap(hdl_type exType, UI ret);
void getCpuTime(cputime_t *global_time, cputime_t *user_time);
void waitForClock();
UI iodevop(UI command, int lineNum, UI deviceNum);
pid_t getPid();

#endif
