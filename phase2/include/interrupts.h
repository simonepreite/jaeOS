#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <initial.h> 


EXTERN void intHandler();
EXTERN void verhogen(int *semaphore, UI status);
EXTERN void deviceHandler(int type);
EXTERN void terminalHandler();

#endif
