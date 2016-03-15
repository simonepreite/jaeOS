#include "../include/const.h"
#include "../include/clist.h"
#include "../include/pcb.h"
#include "../include/asl.h"

HIDDEN struct clist aslh, semdFree;

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
	int trovato = FALSE; 

	if (p != NULL) { 
			if(aslh.next != NULL){ // cerca il semaforo nella lista
	    		clist_foreach(scan, &aslh, s_link, tmp) { 
		    		if (scan->s_semAdd == semAdd) {
   		        		trovato = TRUE;
			    		p->p_cursem->s_semAdd=semAdd;
			    		break;
		    		}
				}
			}
		if (trovato) { // semaforo trovato
			insertProcQ(&scan->s_proc, p); 
			p->p_cursem = scan;
			return FALSE;
		} 
		else { //  semaforo non trovato
			if (semdFree.next != NULL) { // semdFree non vuota
				new_sem = container_of(semdFree.next->next, typeof(*new_sem), s_link); // prendo un nuovo semaforo dalla lista semdFree
				p->p_cursem = new_sem; 
				new_sem->s_semAdd = semAdd;
				new_sem->s_proc.next = NULL;
				clist_dequeue(&semdFree); // decremento della lista dei semafori liberi 
				insertProcQ(&new_sem->s_proc, p);
					clist_foreach(scan, &aslh, s_link, tmp){ 
						if(new_sem->s_semAdd < scan->s_semAdd){
							clist_foreach_add(new_sem, scan, &aslh, s_link, tmp);
							break;
						}
					}
						if (clist_foreach_all(scan, &aslh, s_link, tmp)) { // se la lista è vuota o è stata completamente scorsa
							clist_enqueue(new_sem, &aslh, s_link); 
						}
				return FALSE;
				}
			else { // caso lista semdFree vuota
				return TRUE; 
			}
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
	pcb_t *p;
	semd_t *scan;
	int trovato = 0;
	void *tmp;

	if(aslh.next == NULL) {
		return NULL;
	}
	else{
		clist_foreach(scan, &aslh, s_link, tmp){
			if(scan->s_semAdd == semAdd){
				trovato = 1;
				break;
			}
		}
		if(trovato == 1){
			p = removeProcQ(&scan->s_proc);
			freeSem(scan); // libera il semaforo
			return p;
		}
		else return NULL;
	}
}

/*

rimuove dalla coda dei processi bloccati nel semaforo puntato
da p->p_cursem il processo p.
Se la coda dei processi bloccati risulta vuota dopo la rimozione 
del processo il semaforo viene disattivato e torna alla lista dei
semafori liberi

*/

 pcb_t *outBlocked(pcb_t *p){
	 semd_t *scan;
	 pcb_t *out;

	int trovato = 0;
	void *tmp;
	clist_foreach(scan, &aslh, s_link, tmp) { 
		if (scan->s_semAdd == p->p_cursem->s_semAdd) {
			struct clist *q = &scan->s_proc;
			out = outProcQ(q, p);
			freeSem(scan); // libera il semaforo
			trovato = 1;
			return out;
		}
	}
	if(!trovato) return NULL;
}

/*

ritorna il puntatore alla testa dei processi bloccati in semAdd

*/

 pcb_t *headBlocked(int *semAdd){
	 pcb_t *p;
	semd_t *scan;
	int trovato = 0;
	void *tmp;
	
	if(aslh.next == NULL) {
		return NULL;
	}
	else{
		clist_foreach(scan, &aslh, s_link, tmp){
			if(scan->s_semAdd == semAdd){
				trovato = 1;
				break;
			}
		}
		if(trovato == 1){
			p = clist_head(p, scan->s_proc, p_list);
			return p;
		}
		else return NULL;
	}
}

/*

libera il semaforo che non ha più processi bloccati in coda

*/
void freeSem(semd_t *scan){ 
	void *tmp;
	if(scan->s_proc.next == NULL){
		clist_foreach_delete(scan, &aslh, s_link, tmp);
		clist_enqueue(scan, &semdFree, s_link); 
	}
}