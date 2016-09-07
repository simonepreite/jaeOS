#include <exceptions.h>

// assegno i puntatori alle aree di memoria

int debug_a1;
state_t *tlb_old = (state_t*)TLB_OLDAREA;
state_t *pgmtrap_old = (state_t*)PGMTRAP_OLDAREA;
state_t *sysbp_old = (state_t*)SYSBK_OLDAREA;

/***************************************************************
*                      AUXILIARY FUNCTION                      *
***************************************************************/


void handlerSYSTLBPGM(UI old, UI new, state_t* state){

    if (old == TLB) {
        switch (curProc->tags) {
            case 0:
            case 1:
            case 4:
            case 5:
                terminateProcess(0);
                scheduler();
                break;
            default:
                break;
        }
    }
    else if (old == PGMT) {
        switch (curProc->tags) {
            case 0:
            case 1:
            case 2:
            case 3:
                terminateProcess(0);
                scheduler();
                break;
            default:
                break;
        }
    }
    else if (old == SYS) {
        UI a1 = state->a1;
        UI a2 = state->a2;
        UI a3 = state->a3;
        UI a4 = state->a4;

        debug_a1 = state->a1; //debug istruction

        if (state->CP15_Cause == EXC_SYSCALL) {
            switch (a1) {
                case CREATEPROCESS:
                    curProc->p_s.a1 = createProcess((state_t *)a2);
                    break;
                case TERMINATEPROCESS:
                    terminateProcess(a2);
                    break;
                case SEMOP:
                    semaphoreOperation((int *)a2, a3);
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
                    getCpuTime((cputime_t *)a2, (cputime_t *)a3);
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
        }
    }
    else PANIC();

    if (old != SYS) {
        saveCurState(state, &curProc->excp_state_vector[old]);
        curProc->excp_state_vector[new].a1 = state->CP15_Cause;
        LDST(&(curProc->excp_state_vector[new]));
    }
}

/***************************************************************
*                           HANDLERS                           *
***************************************************************/

void tlbHandler(){
  kernelStart=getTODLO();
  //curProc->global_time += getTODLO() - procInit;
  handlerSYSTLBPGM(TLB, EXCP_TLB_NEW, tlb_old);
  curProc->kernel_mode += getTODLO() - kernelStart; // chiudo qui kernel time perchè in pgmHandler lo rifaccio
  //curProc->global_time += getTODLO() - kernelStart;
}

void sysHandler(){
   kernelStart=getTODLO();
   //curProc->global_time += getTODLO() - procInit;
  /* processo in kernel mode? */
  if(CAUSE_EXCCODE_GET(sysbp_old->CP15_Cause) == EXC_SYSCALL && sysbp_old->a1 >= 1 && sysbp_old->a1 <= 11){

    if((curProc->p_s.cpsr & STATUS_SYS_MODE) == STATUS_SYS_MODE){
      saveCurState(sysbp_old, &curProc->p_s);
      //STST(sysbp_old);
      /* Se l'eccezione è di tipo System call */
      handlerSYSTLBPGM(SYS, EXCP_SYS_NEW, sysbp_old);
      //processo corrente, ricalcolare tempi
      cputime_t end = getTODLO();
        curProc->kernel_mode += end - kernelStart;
        //  curProc->global_time += end - kernelStart;
        /* Richiamo lo scheduler
        if (sysbp_old->a1 == TERMINATEPROCESS && sysbp_old->a2 == (int)curProc)
        scheduler(SCHED_RESET);
        else
          scheduler(SCHED_NEXT);*/
          scheduler();
        }
    /* Altrimenti se è in user-mode */
    else if((curProc->p_s.cpsr & STATUS_USER_MODE) == STATUS_USER_MODE){
      /* Gestisco come fosse una program trap */
      saveCurState(sysbp_old, pgmtrap_old); //fare copy state e non assegnare il puntatore
      /* Setto il registro cause a Reserved Instruction */
      CAUSE_EXCCODE_SET(pgmtrap_old->CP15_Cause, EXC_RESERVEDINSTR);
      cputime_t end = getTODLO();
      curProc->kernel_mode += end - kernelStart;
      //  curProc->global_time += end - kernelStart;
      /* Richiamo l'handler per le pgmtrap */
      pgmHandler();
    }

  }
  else{// significa che è una syscall qualsiasi
    if (curProc->tags != 1 && curProc->tags != 3 && curProc->tags != 5 && curProc->tags != 7){
        terminateProcess(curProc->pid);
        scheduler();
    }
    saveCurState(sysbp_old, &curProc->excp_state_vector[EXCP_SYS_OLD]);
    curProc->excp_state_vector[EXCP_SYS_NEW].a1 = sysbp_old->a1;
    curProc->excp_state_vector[EXCP_SYS_NEW].a2 = sysbp_old->a2;
    curProc->excp_state_vector[EXCP_SYS_NEW].a3 = sysbp_old->a3;
    curProc->excp_state_vector[EXCP_SYS_NEW].a4 = sysbp_old->a4;
    UI temp = sysbp_old->cpsr & 0x0000000F;
    temp = temp << 28;
    curProc->excp_state_vector[EXCP_SYS_NEW].a1 &= 0x0FFFFFFF;
    curProc->excp_state_vector[EXCP_SYS_NEW].a1 |= temp;
    curProc->excp_state_vector[EXCP_SYS_NEW].cpsr = STATUS_ALL_INT_ENABLE(curProc->excp_state_vector[EXCP_SYS_NEW].cpsr);
    curProc->kernel_mode += getTODLO() - kernelStart;
    LDST(&(curProc->excp_state_vector[EXCP_SYS_NEW]));
  }
}

void pgmHandler(){
  kernelStart=getTODLO();
  // curProc->global_time += getTODLO() - procInit;
  handlerSYSTLBPGM(PGMT, EXCP_PGMT_NEW, pgmtrap_old);
  curProc->kernel_mode += getTODLO() - kernelStart; // chiudo qui kernel time perchè in pgmHandler lo rifaccio
  // curProc->global_time += getTODLO() - kernelStart;
}
