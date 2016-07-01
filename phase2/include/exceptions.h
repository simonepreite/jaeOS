#ifndef EXCEPITION_H
#define EXCEPITION_H

#include <const.h>
#include <initial.h>
#include <syscall.h>

EXTERN void pgmHandler();
EXTERN void tlbHandler();
EXTERN void sysHandler();

#endif
