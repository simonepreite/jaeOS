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

// questa funziona si occupa di assegnare pid univoci ai processi

pid_t genPid(){
  //serve argomento, magari una variabile condivisa
  /* code */

}

int createProcess(state_t *stato){
  pcb_t *newProc = allocPcb();

  if(newProc != NULL) return -1; // fail

  saveCurState(stato, &(newProc->p_s));

  processCounter++;

  insertChild(curProc, newProc);
  newProc->pid = genPid();
  insertProcQ(readyQueue, newProc);

  return 0; // Success
}

void terminateProcess(pid_t p){
  //work in progress
  /*devo trovare il pid nella lista dei processi
  fare containerof per avere il puntatore del processo
  vittima e killare tutta la sua progenie
  */
  if(p != 0){
    while(!emptyChild((pcb_t*)p)){
      terminateProcess((int)removeChild((pcb_t*)p)->pid);
    }
    outChild((pcb_t*)p);

    if(((pcb_t*)p)->p_cursem){
      if (!outBlocked((pcb_t*)p)) PANIC();
      softBlockCounter--;
    }
    outProcQ(readyQueue, (pcb_t*)p);
    processCounter--;

    if((pcb_t*)p == curProc){
      curProc = NULL;
    }
  }
}

void semaphoreOperation(int *sem, int weight){
  if(weight == 1){
    pcb_t *firstBlock;
    (*sem)++;
    firstBlock = removeBlocked(sem);
    if(firstBlock != NULL){
      insertProcQ(readyQueue, firstBlock);
      //decremento di softBlockCounter se sono su un semaforo soft
      firstBlock->p_cursem = NULL;
    }
  }
  else if (weight == -1){
    (*sem)--;
    if(*sem < 0){
      //inserire kernel time su nuovi elementi della struttura
      if(insertBlocked(sem, curProc)!=0) PANIC();
      //controllare incremento di softBlockCounter solo su semafori soft
      //dire allo scheduler di caricare il processo successivo
      curProc = NULL;
    }
  }
  else if (weight == 0){
    terminateProcess(curProc->pid);
  }
  else PANIC();
}

pid_t getPid(){
  return curProc->pid;
}
