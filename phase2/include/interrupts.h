#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <initial.h>
// debug

EXTERN UI control100ms;

EXTERN void testfun();

// interrupts handler

void intHandler();
void deviceHandler(int type);
void terminalHandler();
void timerHandler();

#endif
