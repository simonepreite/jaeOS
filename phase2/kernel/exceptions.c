#include <exceptions.h>

// assegno i puntatori alle aree di memoria

state_t *tlb_old = (state_t*)TLB_OLDAREA;
state_t *pgmtrap_old = (state_t*)PGMTRAP_OLDAREA;
state_t *sysbp_old = (state_t*)SYSBK_OLDAREA;

/***************************************************************
*                      AUXILIARY FUNCTION                      *
***************************************************************/


void handlerSYSTLBPGM(UI old, UI new, state_t* state){
  switch(new){
    case TLB:{
      if(curProc->tags != 2||3||6||7)
          terminateProcess(0);
          scheduler(SCHED_NEXT);
      break;
    }
    case PGMT:{
      if(curProc->tags != 4||5||6||7)
          terminateProcess(0);
          scheduler(SCHED_NEXT);
      break;
    }
    // forse riesco ad usarla anche come handler per syscall
  }
  saveCurState(&curProc->excp_state_vector[old], state);
  curProc->excp_state_vector[new].a1 = state->CP15_Cause;
  curProc->excp_state_vector[new].cpsr = STATUS_ALL_INT_ENABLE(curProc->excp_state_vector[new].cpsr);
  LDST(&(curProc->excp_state_vector[new]));
}

/***************************************************************
*                           HANDLERS                           *
***************************************************************/

void tlbHandler(){
  kernelStart=getTODLO();
  handlerSYSTLBPGM(TLB, EXCP_TLB_NEW, tlb_old);
  curProc->kernel_mode = getTODLO() - kernelStart;
}

void sysHandler(){
  /* processo in kernel mode? */
  if((curProc->p_s.cpsr & STATUS_SYS_MODE) == STATUS_SYS_MODE){
    STST(sysbp_old);
    UI cause = sysbp_old->CP15_Cause;
    UI a1 = sysbp_old->a1;
    UI a2 = sysbp_old->a2;
    UI a3 = sysbp_old->a3;
    UI a4 = sysbp_old->a4;
    kernelStart=getTODLO();
    /* Se l'eccezione è di tipo System call */
    if(cause==EXC_SYSCALL){
      /* Se è fra SYS1 e SYS8 richiama le funzioni adeguate */
      switch(a1){
        case CREATEPROCESS:
          curProc->p_s.a1 = createProcess((state_t *)a2);
        break;
        case TERMINATEPROCESS:
          terminateProcess(a2);
        break;
        case SEMOP:
          semaphoreOperation((int*)a2, a3);
        break;
        case SPECSYSHDL:
          specifySysBp(a2, a3, a4);
          break;
        case SPECTLBHDL:
          specifyTlb(a2, a3, a4);
          break;
        case SPECPGMTHDL:
          specifyPgm(a2, a3, a4);
          break;
        case EXITTRAP:
          exitTrap(a2, a3);
          break;
        case GETCPUTIME:
          getCpuTime((cputime_t*)a2, (cputime_t*)a3);
        break;
        case WAITCLOCK:
          waitForClock();
          break;
        case IODEVOP:
          curProc->p_s.a1 = iodevop(a2, a3, a4);
          break;
        case GETPID:
          curProc->p_s.a1 = getPid();
          break;
        default:
          PANIC();
      }
    }else PANIC(); //non necessario insieme al controllo su cause
    //processo corrente, ricalcolare tempi
    curProc->kernel_mode = getTODLO() - kernelStart;
    /* Richiamo lo scheduler */
    scheduler(SCHED_CONTINUE);//non è detto che sia continue il flag, se chiamo la terminate sul processo stesso devo fare reset
    /* Altrimenti se è in user-mode */
  } else if((curProc->p_s.cpsr & STATUS_USER_MODE) == STATUS_USER_MODE){
    /* Gestisco come fosse una program trap */
    pgmtrap_old=sysbp_old;
    /* Setto il registro cause a Reserved Instruction */
    pgmtrap_old->CP15_Cause=EXC_RESERVEDINSTR;
    /* Richiamo l'handler per le pgmtrap */
    pgmHandler();
  }
}

void pgmHandler(){
  kernelStart=getTODLO();
  handlerSYSTLBPGM(PGMT, EXCP_PGMT_NEW, pgmtrap_old);
  curProc->kernel_mode = getTODLO() - kernelStart;
}
