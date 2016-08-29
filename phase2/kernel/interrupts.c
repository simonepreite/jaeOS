#include <interrupts.h>


void intHandler(){
  kernelStart=getTODLO();

  curProc->kernel_mode = getTODLO() - kernelStart;
}

void verhogen(int *semaphore, UI status) {
	pcb_t *unblocked = removeBlocked(semaphore);
	(*semaphore)++;
	if (unblocked) {
		insertProcQ(&readyQueue, unblocked);
		unblocked->p_cursem = NULL;
		unblocked->p_s.a1 = status;
	}
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

void deviceHandler(int type) {
	UI *bitmapForLine = (UI *)CDEV_BITMAP_ADDR(type);								// ottengo la bitmap delle linee in base al tipo di device
	UI deviceNumber = getDeviceNumberFromLineBitmap(bitmapForLine);					// ottengo il numero del device in base alla bitmap (vedere funzione ausiliaria)
	devreg_t *deviceRegister = (devreg_t *)DEV_REG_ADDR(type, deviceNumber);		// ottengo il device register per il device da gestire
	deviceRegister->dtp.command = DEV_C_ACK;										// setto l'acknowledgement come comando per il dispositivo
	
	UI index = EXT_IL_INDEX(type) * N_DEV_PER_IL + deviceNumber;					// calcolo l'indice del semaforo associato al dispositivo
	if (semDevices[index] < 1) {													// se il valore del semaforo è > 1
		pcb_t *process = headBlocked(&semDevices[index]);							// ottengo un puntatore al primo processo bloccato su tale semaforo
		process->p_s.a1 = deviceRegister->dtp.status;								// aggiorno lo stato nel nuovo processo (vedi specifiche)
		semaphoreOperation(&semDevices[index], 1);									// eseguo una SYS3 con peso 1 (vedi specifiche) sul semaforo
	}
}

// DA OTTIMIZZARE IL RIUSO DEL CODICE

void terminalHandler() {
	UI *bitmapForLine = (UI *)CDEV_BITMAP_ADDR(INT_TERMINAL);
	UI terminalNumber = getDeviceNumberFromLineBitmap(bitmapForLine);
	devreg_t *terminalRegister = (devreg_t *)DEV_REG_ADDR(INT_TERMINAL, terminalNumber);
	UI index = 0;

	if ((terminalRegister->term.transm_status & 0x000000FF) == DEV_TRCV_S_CHARTRSM) {
		index = EXT_IL_INDEX(INT_TERMINAL) * N_DEV_PER_IL + terminalNumber;
		if (semDevices[index] < 1) {
			pcb_t *process = headBlocked(&semDevices[index]);
			process->p_s.a1 = terminalRegister->term.transm_status;
			semaphoreOperation(&semDevices[index], 1);
		}
		terminalRegister->term.transm_command = DEV_C_ACK;
	}
	else if ((terminalRegister->term.recv_status & 0x000000FF) == DEV_TRCV_S_CHARRECV) {
		index = EXT_IL_INDEX(INT_TERMINAL) * N_DEV_PER_IL + N_DEV_PER_IL + terminalNumber;
		if (semDevices[index] < 1) {
			pcb_t *process = headBlocked(&semDevices[index]);
			process->p_s.a1 = terminalRegister->term.recv_status;
			semaphoreOperation(&semDevices[index], 1);
		}
		terminalRegister->term.recv_command = DEV_C_ACK;
	}
}
