#include <syscall.h>


void saveCurState(state_t *state, state_t *newState){
  newState->a1 = state->a1;
  newState->a2 = state->a2;
  newState->a3 = state->a3;
  newState->a4 = state->a4;
  newState->v1 = state->v1;
  newState->v2 = state->v2;
  newState->v3 = state->v3;
  newState->v4 = state->v4;
  newState->v5 = state->v5;
  newState->v6 = state->v6;
  newState->sl = state->sl;
  newState->fp = state->fp;
  newState->ip = state->ip;
  newState->sp = state->sp;
  newState->lr = state->lr;
  newState->pc = state->pc;
  newState->cpsr = state->cpsr;
  newState->CP15_Control = state->CP15_Control;
  newState->CP15_EntryHi = state->CP15_EntryHi;
  newState->CP15_Cause = state->CP15_Cause;
  newState->TOD_Hi = state->TOD_Hi;
  newState->TOD_Low = state->TOD_Low;
}

/***************************************************************
*                     SYSCALL KERNEL MODE                      *
***************************************************************/

int createProcess(state_t *stato){
  pcb_t *newProc = allocPcb();

  if(newProc!=NULL) return -1; // fail

  saveCurState(stato, &(newProc->p_s));

  processCounter++;

  insertChild(curProc, newProc);
  insertProcQ(&readyQueue, newProc);

  return 0; // Success
}

//work in progress

void terminateProcess(pcb_t *p){
  if(p != NULL){
    while(!emptyChild(p)){
      terminateProcess(removeChild(p));
    }
    outChild(p);

    if(p->p_cursem){
      if (!outBlocked(p)) PANIC();
      softBlockCounter--;
    }
    outProcQ(&readyQueue, p);
    processCounter--;

    if(p==curProc){
       curProc=NULL;
    }
  }

}
