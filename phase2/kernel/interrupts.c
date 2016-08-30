#include <interrupts.h>


void intHandler(){
	setSTATUS(STATUS_ALL_INT_DISABLE(getSTATUS()));
	kernelStart=getTODLO();

	state_t *oldState = (state_t *)INT_OLDAREA;
	if (curProc) {
		(*oldState).pc -= WORD_SIZE;
		saveCurState(oldState, curProc->p_s);
	}

	UI cause = getCAUSE();

	if (CAUSE_IP_GET(cause, IL_TIMER)) timerHandler();
	else if (CAUSE_IP_GET(cause, IL_DISK)) deviceHandler(IL_DISK);
	else if (CAUSE_IP_GET(cause, IL_TAPE)) deviceHandler(IL_TAPE);
	else if (CAUSE_IP_GET(cause, IL_ETHERNET)) deviceHandler(IL_ETHERNET);
	else if (CAUSE_IP_GET(cause, IL_PRINTER)) deviceHandler(IL_PRINTER);
	else if (CAUSE_IP_GET(cause, IL_TERMINAL)) terminalHandler();

	if (curProc)
		curProc->kernel_mode = getTODLO() - kernelStart;

	setSTATUS(STATUS_ALL_INT_ENABLE(getSTATUS()));
	scheduler(SCHED_NEXT);
}

UI getDeviceNumberFromLineBitmap(int *lineAddr) {
	int activeBit = 0x00000001;
	int i;

	for (i = 0; i < 8; i++) {
		if ((*lineAddr & activeBit) == activeBit) break;
		activeBit = activeBit << 1;
	}

	return i;
}

void acknowledge(UI semIndex, devreg_t *devRegister, ack_type type) {
	if (semDevices[semIndex] < 1) {
		pcb_t *process = headBlocked(&semDevices[semIndex]);
		switch (type) {
			case ACK_GEN_DEVICE:
				process->p_s.a1 = devRegister->dtp.status;
				devRegister->dtp.command = DEV_C_ACK;
				break;
			case ACK_TERM_TRANSMIT:
				process->p_s.a1 = devRegister->term.transm_status;
				devRegister->term.transm_command = DEV_C_ACK;
				break;
			case ACK_TERM_RECEIVE:
				process->p_s.a1 = devRegister->term.recv_status;
				devRegister->term.recv_command = DEV_C_ACK;
				break;
			default:
				PANIC();
				break;
		}
		semaphoreOperation(&semDevices[semIndex], 1);
	}
}

void deviceHandler(int type) {
	UI *bitmapForLine = (UI *)CDEV_BITMAP_ADDR(type);								// ottengo la bitmap delle linee in base al tipo di device
	UI deviceNumber = getDeviceNumberFromLineBitmap(bitmapForLine);					// ottengo il numero del device in base alla bitmap (vedere funzione ausiliaria)
	devreg_t *deviceRegister = (devreg_t *)DEV_REG_ADDR(type, deviceNumber);		// ottengo il device register per il device da gestire
	
	UI index = EXT_IL_INDEX(type) * N_DEV_PER_IL + deviceNumber;					// calcolo l'indice del semaforo associato al dispositivo
	acknowledge(index, deviceRegister, ACK_GEN_DEVICE);
}

void terminalHandler() {
	UI *bitmapForLine = (UI *)CDEV_BITMAP_ADDR(INT_TERMINAL);
	UI terminalNumber = getDeviceNumberFromLineBitmap(bitmapForLine);
	devreg_t *terminalRegister = (devreg_t *)DEV_REG_ADDR(INT_TERMINAL, terminalNumber);
	UI index = 0;

	// Trasmissione Carattere, operazione a proprità più elevata rispetto alla ricezione
	if ((terminalRegister->term.transm_status & 0x0000000F) == DEV_TTRS_S_CHARTRSM) {
		index = EXT_IL_INDEX(INT_TERMINAL) * N_DEV_PER_IL + terminalNumber;
		acknowledge(index, terminalRegister, ACK_TERM_TRANSMIT);
	}
	// Ricezione Carattere
	else if ((terminalRegister->term.recv_status & 0x0000000F) == DEV_TRCV_S_CHARRECV) {
		index = EXT_IL_INDEX(INT_TERMINAL) * N_DEV_PER_IL + N_DEV_PER_IL + terminalNumber;
		acknowledge(index, terminalRegister, ACK_TERM_RECEIVE);
	}
}

void timerHandler() {
	// Bisogna capire la gestione dei tempi e dello pseudoclock -.-"
}
