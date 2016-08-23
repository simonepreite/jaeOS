#include <interrupts.h>


void intHandler(){
  kernelStart=getTODLO();

  curProc->kernel_mode = getTODLO() - kernelStart;
}
