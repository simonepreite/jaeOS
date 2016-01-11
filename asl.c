#include "include/const.h"
#include "include/clist.h"
#include "include/pcb.h"
#include "include/asl.h"

HIDDEN struct clist aslh, semdFree;

/***************************************************************
*                    ACTIVE SEMAPHORE LIST                     *
***************************************************************/

//Inizializza la lista semdFree, verrà chiamato solo una volta

void initASL(){
	static struct semd_t semdTable[MAXPROC];   
	int i;
	aslh.next = NULL;
	semdFree.next = &semdTable[MAXPROC-1].s_link;
	semdTable[MAXPROC-1].s_link.next = &semdTable[0].s_link;
	for(i = 0; i < (MAXPROC-1); i++){
		semdTable[i].s_link.next = &semdTable[i+1].s_link;
	}
}



int insertBlocked(int *semAdd, struct pcb_t *p);
	/* 
	1° caso: 
		cerca il semaforo nella lista aslh e nel momento in cui lo trova
		inserisce il processo puntato da p nella lista dei processi
		bloccati in quel semaforo e setta il puntatore al semaforo
		a semAdd
	2° caso:
		se il semaforo non c'è e la lista semdFree è vuota ritorna TRUE
	3° caso:
		se il semaforo non c'è lo alloca prendendono dalla lista semdfree



struct pcb_t *removeBlocked(int *semAdd);



struct pcb_t *outBlocked(struct pcb_t *p); 



struct pcb_t *headBlocked(int *semAdd);
