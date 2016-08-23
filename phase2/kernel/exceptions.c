#include <exceptions.h>

// probabilemnte questo assegnamento è da eliminare e va rifatto all'interno
// dei rispettivi handler
state_t *tlb_old = (state_t*)TLB_OLDAREA;
state_t *pgmtrap_old = (state_t*)PGMTRAP_OLDAREA;
state_t *sysbp_old = (state_t*)SYSBK_OLDAREA;


void pgmHandler(){
  kernelStart=getTODLO();

  curProc->kernel_mode = getTODLO() - kernelStart;
}

void tlbHandler(){
  kernelStart=getTODLO();

  curProc->kernel_mode = getTODLO() - kernelStart;
}

void sysHandler(){
  /* processo in kernel mode? */
  if((curProc->p_s.cpsr & STATUS_SYS_MODE) == STATUS_SYS_MODE){
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
          //function
          break;
        case SPECTLBHDL:
          //function
          break;
        case SPECPGMTHDL:
          //function
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
    scheduler(SCHED_CONTINUE);
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
