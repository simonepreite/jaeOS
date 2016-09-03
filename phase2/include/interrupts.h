#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <initial.h>
// debug

EXTERN void testfun();

// interrupts handler

void intHandler();
void deviceHandler(int type);
void terminalHandler();
void timerHandler();

#endif
