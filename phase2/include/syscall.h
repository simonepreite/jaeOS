#ifndef SYSCALL_H
#define SYSCALL_H

//#include <types.h>
#include <initial.h>

EXTERN void saveCurState(state_t *state, state_t *newState);
EXTERN int createProcess(state_t *stato);
EXTERN void terminateProcess(pcb_t *p);

#endif
