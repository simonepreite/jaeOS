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

	ovviamente non funziona ancora probabilmente concettualemte sbagliato
	come ragionamento da ripensare totalmente.
*/

int insertBlocked(int *semAdd, struct pcb_t *p){
	struct semd_t *scan, *addr;
	void *tmp;
	int trovato = 0;
	clist_foreach(scan, &aslh, s_link, tmp){
		if(semAdd == scan->s_semAdd){
			clist_enqueue(p, &aslh, p_list);
			addr = container_of(semAdd, typeof(*addr), s_proc);
			p->p_cursem = addr; 
			trovato = 1;
			return FALSE;
		}
	}
	if(semdFree.next != NULL){
		clist_foreach(scan, &aslh, s_link, tmp) {
			if (semAdd < scan->s_semAdd) {
				dequeue(semdFree);
				clist_foreach_add(semAdd, scan, &aslh, s_link, tmp);
				}
		}
			if (clist_foreach_all(scan, &aslh, s_link,tmp)) 
				clist_enqueue(semAdd, &myclist, s_link);
		scan->s_semAdd = semAdd;
		clist_enqueue(p, &aslh, s_proc);
		addr = container_of((semAdd), typeof(*addr), s_proc);
		p->p_cursem = addr; 
		return FALSE;
	}
	else return TRUE;
}

struct pcb_t *removeBlocked(int *semAdd);



struct pcb_t *outBlocked(struct pcb_t *p); 



struct pcb_t *headBlocked(int *semAdd);
