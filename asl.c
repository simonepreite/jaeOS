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

NON È ANCORA DETTO CHE SIA PRIVA DI PROBLEMI!!!!!!!!!!!

*/

int insertBlocked(int *semAdd, struct pcb_t *p){
	struct semd_t *new_sem, *scan;
	int trovato = 0;
	void *tmp;
	char *d = "trovato sem e inserito proc\n";
	char *e = "sem non trovato e semdFree non vuota\n";
	char *f = "non trovato e semdFree vuota\n";
	char *g = "semaforo inserito\n";
	char *a = "sono nell'if\n";
	char *c = "sono nel secondo if\n";
	char *b = "non ho fatto nulla\n";
	if(aslh.next != NULL){
		tprint(a);
		scan = container_of(aslh.next, typeof(*scan), s_link);
		clist_foreach(scan, &aslh, s_link, tmp){
			if(scan->s_semAdd == semAdd){
				trovato = 1;
				break;
			}
		}
		if(trovato == 1){
			clist_enqueue(p, &scan->s_proc, p_list);
			p->p_cursem = scan;
			tprint(d);
			return FALSE;
		}
		else if (trovato == 0 && semdFree.next != NULL){
			tprint(e);
			scan = container_of(aslh.next, typeof(*scan), s_link);
			p->p_cursem = scan;
			clist_foreach(scan, &aslh, s_link, tmp){
				if(semAdd < scan->s_semAdd){
					new_sem = container_of(semdFree.next, typeof(*new_sem), s_link);
					clist_enqueue(p, &new_sem->s_proc, p_list);
					clist_dequeue(&semdFree);
					clist_foreach_add(new_sem, scan, &aslh, s_link, tmp);
					tprint(g);
					break;
				}
			}
			if (clist_foreach_all(scan, &aslh, s_link, tmp)) clist_enqueue(new_sem, &aslh, s_link);
		return FALSE;
		}
	}
	else if(aslh.next == NULL && semdFree.next != NULL){
		tprint(c);
		scan = container_of(semdFree.next, typeof(*scan), s_link);
			clist_dequeue(&semdFree);
			clist_enqueue(p, &scan->s_proc, p_list);
			clist_enqueue(scan, &aslh, s_link);
			p->p_cursem = scan;
	return FALSE;
	}
	else {
		tprint(b);
		return TRUE;
	}	
}
		/*clist_foreach(scan, &aslh, s_link, tmp) {
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
*/
struct pcb_t *removeBlocked(int *semAdd){
	struct pcb_t *p;
	struct semd_t *scan;
	int trovato = 0;
	void *tmp;
	char *a = "aslh vuota\n";
	char *b = "cerco il semaforo\n";
	char *c = "semaforo non trovato\n";
	if(aslh.next == NULL) {
		tprint(a);
		return NULL;
	}
	else{
		scan = container_of(aslh.next, typeof(*scan), s_link);
		clist_foreach(scan, &aslh, s_link, tmp){
			tprint(b);
			if(scan->s_semAdd == semAdd){
				trovato = 1;
				break;
			}
		}
		if(trovato == 1){
			p = container_of(scan->s_proc.next, typeof(*p), p_list);
			clist_dequeue(scan->s_proc.next);
			return p;
		}
		else return NULL;
	}

}



struct pcb_t *outBlocked(struct pcb_t *p); 



struct pcb_t *headBlocked(int *semAdd);
