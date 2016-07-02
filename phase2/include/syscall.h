#ifndef SYSCALL_H
#define SYSCALL_H

#include <types.h>

EXTERN void createProcess(state_t *stato);
EXTERN void terminateProcess(pcb_t *p);

#endif
