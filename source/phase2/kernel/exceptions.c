/*
 *	EXCEPTIONS.C
 *	TLB, PgmTrap and SYS/Bp Exception Handlers Implementation File
 *
 *	Gruppo 28:
 *	Del Vecchio Matteo
 *	Preite Simone
 */

#include <exceptions.h>

// assegno i puntatori alle aree di memoria
state_t *tlb_old = (state_t*)TLB_OLDAREA;
state_t *pgmtrap_old = (state_t*)PGMTRAP_OLDAREA;
state_t *sysbp_old = (state_t*)SYSBK_OLDAREA;

/***************************************************************
*                      AUXILIARY FUNCTION                      *
***************************************************************/

void handlerSYSTLBPGM(hdl_type old, UI new, state_t* state) {
	if (old == TLB_HDL) {
		if (curProc->tags == 0 || curProc->tags == 1 || curProc->tags == 4 || curProc->tags == 5) {
			terminateProcess(0);
			scheduler();
		}
	}
	else if (old == PGMT_HDL) {
		if (curProc->tags == 0 || curProc->tags == 1 || curProc->tags == 2 || curProc->tags == 3) {
			terminateProcess(0);
			scheduler();
		}
	}
	else if (old == SYS_HDL) {
		UI a1 = state->a1;
		UI a2 = state->a2;
		UI a3 = state->a3;
		UI a4 = state->a4;

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
					kernelStart = getTODLO();
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

	if (old != SYS_HDL) {
		saveCurState(state, &(curProc->excp_state_vector[old]));
		curProc->excp_state_vector[new].a1 = CAUSE_EXCCODE_GET(state->CP15_Cause);
		curProc->kernel_mode += getTODLO() - kernelStart; // chiudo qui kernel time perchè in pgmHandler lo rifaccio
		LDST(&(curProc->excp_state_vector[new]));
	}
}

/***************************************************************
*                           HANDLERS                           *
***************************************************************/

void tlbHandler() {
	kernelStart = getTODLO();
	handlerSYSTLBPGM(TLB_HDL, EXCP_TLB_NEW, tlb_old);
}

void sysHandler() {
	kernelStart = getTODLO();
	/* processo in kernel mode? */
	if (CAUSE_EXCCODE_GET(sysbp_old->CP15_Cause) == EXC_SYSCALL && sysbp_old->a1 >= 1 && sysbp_old->a1 <= 11) {

		if ((curProc->p_s.cpsr & STATUS_SYS_MODE) == STATUS_SYS_MODE) {
			saveCurState(sysbp_old, &curProc->p_s);
			/* Se l'eccezione è di tipo System call */
			handlerSYSTLBPGM(SYS_HDL, EXCP_SYS_NEW, sysbp_old);
			//processo corrente, ricalcolare tempi
			curProc->kernel_mode += getTODLO() - kernelStart;
			/* Richiamo lo scheduler */
			scheduler();
		}
		/* Altrimenti se è in user-mode */
		else if ((curProc->p_s.cpsr & STATUS_USER_MODE) == STATUS_USER_MODE) {
			/* Gestisco come fosse una program trap */
			saveCurState(sysbp_old, pgmtrap_old);
			/* Setto il registro cause a Reserved Instruction */
			pgmtrap_old->CP15_Cause = CAUSE_EXCCODE_SET(pgmtrap_old->CP15_Cause, EXC_RESERVEDINSTR);
			curProc->kernel_mode += getTODLO() - kernelStart;
			/* Richiamo l'handler per le pgmtrap */
			pgmHandler();
		}
	}
	else {	// significa che è una syscall qualsiasi
		if (curProc->tags != 1 && curProc->tags != 3 && curProc->tags != 5 && curProc->tags != 7) {
			terminateProcess(curProc->pid);
			scheduler();
		}

		saveCurState(sysbp_old, &(curProc->excp_state_vector[EXCP_SYS_OLD]));

		curProc->excp_state_vector[EXCP_SYS_NEW].a1 = sysbp_old->a1;
		curProc->excp_state_vector[EXCP_SYS_NEW].a2 = sysbp_old->a2;
		curProc->excp_state_vector[EXCP_SYS_NEW].a3 = sysbp_old->a3;
		curProc->excp_state_vector[EXCP_SYS_NEW].a4 = sysbp_old->a4;
		curProc->excp_state_vector[EXCP_SYS_NEW].a1 &= 0x0FFFFFFF;
		curProc->excp_state_vector[EXCP_SYS_NEW].a1 |= ((sysbp_old->cpsr & 0x0000000F) << 28);

		curProc->kernel_mode += getTODLO() - kernelStart;
		LDST(&(curProc->excp_state_vector[EXCP_SYS_NEW]));
	}
}

void pgmHandler() {
	kernelStart = getTODLO();
	handlerSYSTLBPGM(PGMT_HDL, EXCP_PGMT_NEW, pgmtrap_old);
}
