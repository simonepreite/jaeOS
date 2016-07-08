#include <exceptions.h>

state_t *tlb_old = (state_t*) TLB_OLDAREA;
state_t *pgmtrap_old = (state_t*) PGMTRAP_OLDAREA;
state_t *sysbp_old = (state_t*) SYSBK_OLDAREA;


void pgmHandler(){
}

void tlbHandler(){
}

void sysHandler(){
    /* Se il processo è in kernel mode gestisce adeguatamente */
    if((curProc->p_s.cpsr & STATUS_SYS_MODE) == STATUS_SYS_MODE){
	    unsigned int cause = sysbp_old->CP15_Cause;
		  unsigned int a1 = sysbp_old->a1;//rivedere con specifiche nuove
		  unsigned int a2 = sysbp_old->a2;//rivedere con specifiche nuove
		  unsigned int a3 = sysbp_old->a3;//rivedere con specifiche nuove
		  unsigned int a4 = sysbp_old->a4;//rivedere con specifiche nuove
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
		        	//function
		        	  break;
	            case GETCPUTIME:
	            	//function
		            break;
	            case WAITCLOCK:
	            	//function
		            break;
	            case IODEVOP:
	            	//function
		            break;
		          case GETPID:
                curProc->p_s.a1 = getPid();
		        	//function
		        	  break;
		        /* Altrimenti la gestione viene passata in alto */
		        default:
		            //useExStVec(SPECSYSBP);
		            break;

		    }
		/* Se l'eccezione è di tipo breakpoint viene passata in alto */
		} else if(cause==EXC_BREAKPOINT){
		    //useExStVec(SPECSYSBP);
		/* Caso particolare */
		} else {
		    PANIC();
		}

	    /* Richiamo lo scheduler */
	    scheduler();
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
