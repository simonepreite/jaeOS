#include "const.h"
#include "clist.h"
#include "pcb.h"
#include "asl.h"

/* struttura semafori */

typedef struct semd_t{
	int *s_semAdd; 
	struct clist s_link; 
	struct clist s_proc; 
}semd_t;


HIDDEN struct clist aslh, semdFree = CLIST_INIT;

/***************************************************************
*                    ACTIVE SEMAPHORE LIST                     *
***************************************************************/

//Inizializza la lista semdFree, verrà chiamato solo una volta

void initASL(){
	static semd_t semdTable[MAXPROC];   
	int i;

	aslh.next = NULL;
	for(i = 0; i < (MAXPROC); i++){
		semd_t *q = &semdTable[i];
		clist_enqueue(q, &semdFree, s_link); 
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
	se il semaforo non c'è lo alloca prendendolo dalla lista semdfree

*/

int insertBlocked(int *semAdd, pcb_t *p){
    void *tmp;
	semd_t *scan, *new_sem;

	if(aslh.next != NULL || semdFree.next != NULL){ 
	    clist_foreach(scan, &aslh, s_link, tmp) { 
		    if (scan->s_semAdd == semAdd) { //
   		        insertProcQ(&scan->s_proc, p); 
				p->p_cursem = scan;
				return FALSE;
		    }
		    if(semAdd < scan->s_semAdd){
		    	if((new_sem = new_semaphore(new_sem, p, semAdd))==NULL) return TRUE; // alloca un nuovo semaforo
		    	clist_foreach_add(new_sem, scan, &aslh, s_link, tmp);
				return FALSE;		    		
			}
		}
			if (clist_foreach_all(scan, &aslh, s_link, tmp)) { 
				if((new_sem=new_semaphore(new_sem, p, semAdd))==NULL) return TRUE; 
				clist_enqueue(new_sem, &aslh, s_link); 
				return FALSE;
			}
	}
}

/*

rimuove il primo processo dalla coda dei processi bloccati nel 
semaforo puntato da semAdd.
Se la coda dei processi bloccati risulta vuota dopo la rimozione 
del processo il semaforo viene disattivato e torna alla lista dei
semafori liberi

*/

pcb_t *removeBlocked(int *semAdd){
	pcb_t *p = NULL;
	semd_t *scan = NULL;
	void *tmp;

	if(aslh.next == NULL) {
		return NULL;
	}
	else{
		clist_foreach(scan, &aslh, s_link, tmp){
			if( scan->s_semAdd <= semAdd){
				if(scan->s_semAdd == semAdd){
					p = removeProcQ(&scan->s_proc);
					if(scan->s_proc.next==NULL){
						clist_foreach_delete(p->p_cursem, &aslh, s_link, tmp);
						clist_enqueue(p->p_cursem, &semdFree, s_link); 
					}
					return p;
				}
			}
		}
	return p;
	}
}

/*

rimuove dalla coda dei processi bloccati nel semaforo puntato
da p->p_cursem il processo p.
Se la coda dei processi bloccati risulta vuota dopo la rimozione 
del processo il semaforo viene disattivato e torna alla lista dei
semafori liberi

*/

//non stabile
 pcb_t *outBlocked(pcb_t *p){
	 pcb_t *out = NULL;
	 void *tmp;
	 struct clist *q = &p->p_cursem->s_proc;
	
	if(q->next!=NULL){
			out = outProcQ(q, p);
			if(p->p_cursem->s_proc.next==NULL){
				clist_delete(p->p_cursem, &aslh, s_link);
				clist_enqueue(p->p_cursem, &semdFree, s_link); 
			}
	}
	return out;
}

//ritorna il puntatore alla testa dei processi bloccati in semAdd

 pcb_t *headBlocked(int *semAdd){
	 pcb_t *p = NULL;
	semd_t *scan;
	void *tmp;
	
	if(aslh.next == NULL) {
		return NULL;
	}
	else{
		clist_foreach(scan, &aslh, s_link, tmp){
			if(scan->s_semAdd == semAdd){
				p = clist_head(p, scan->s_proc, p_list);
				return p;
			}
		}
	return p;
	}
}


/***************************************************************
*                      AUXILIARY FUNCTION                      *
***************************************************************/

/*

prende un semaphoro dalla lista dei semdFree e lo attiva 
ammesso che la lista dei semafori liberi non sia vuota

*/

semd_t *new_semaphore(semd_t *new_sem, pcb_t *p, int *semAdd){
	new_sem = NULL;
	if(!clist_empty(semdFree)){
		new_sem = clist_head( new_sem, semdFree, s_link);//container_of(semdFree.next->next, typeof(*new_sem), s_link); // prendo un nuovo semaforo dalla lista semdFree
		p->p_cursem = new_sem; 
		new_sem->s_semAdd = semAdd;
		new_sem->s_proc.next = NULL;
		clist_dequeue(&semdFree); // decremento della lista dei semafori liberi 
		insertProcQ(&new_sem->s_proc, p);
	}
	return new_sem;
}