#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <initial.h> 


EXTERN void intHandler();
EXTERN void deviceHandler(int type);
EXTERN void terminalHandler();
EXTERN void timerHandler();

#endif
