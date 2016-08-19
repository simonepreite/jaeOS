#ifndef EXCEPITION_H
#define EXCEPITION_H

#include <const.h>
#include <initial.h>
#include <syscall.h>

void pgmHandler();
void tlbHandler();
void sysHandler();
EXTERN void saveCurState(state_t *state, state_t *newState);

#endif
