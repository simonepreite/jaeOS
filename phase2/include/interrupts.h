#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <initial.h>

EXTERN void testfun();

// auxiliary variables

EXTERN cputime_t executed;

// interrupts handler

void intHandler();
void deviceHandler(int type);
void terminalHandler();
void timerHandler();

#endif
