#include "include/const.h"
#include "include/clist.h"
#include "include/pcb.h"
#include "include/asl.h"

HIDDEN struct clist aslh, semdFree;

/***************************************************************
*                    ACTIVE SEMAPHORE LIST                     *
***************************************************************/

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

int insertBlocked(int *semAdd, struct pcb_t *p){
	int trovato = 0;
	void *tmp;
	struct semd_t *scan, *elem;
	scan = container_of(aslh.next->next, typeof(*scan), s_link);
	elem->s_semAdd = semAdd;
	clist_foreach(scan, &aslh, s_link, tmp) {
		if(semAdd == scan->s_semAdd) trovato = 1;
	}
	if(semdFree.next == NULL && trovato == 0) return TRUE;
	else if(trovato == 1){
		clist_foreach(scan, &aslh, s_link, tmp){
			if (semAdd < scan->s_semAdd){
				insertProcQ(&aslh, p); 
				return FALSE;
			}
		}
	}
	else{
		struct semd_t *sem_temp;
		sem_temp = container_of(semdFree.next->next, typeof(*sem_temp), s_link); 
		clist_dequeue(&semdFree);
		clist_push(sem_temp, &aslh, s_link);
		insertProcQ(&aslh, p);
		return FALSE;
	}
}

