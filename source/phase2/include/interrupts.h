/*
 *	INTERRUPTS.H
 *	Device Interrupt Exception Handlers Header File
 *
 *	Gruppo 28:
 *	Del Vecchio Matteo
 *	Preite Simone
 */

#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <initial.h>

// interrupts handler

void intHandler();
void deviceHandler(int type);
void terminalHandler();
void timerHandler();

#endif
