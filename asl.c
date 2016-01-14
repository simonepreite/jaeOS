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

PROBLEMA NELLA DEQUEUE DI semdFree che scende troppo in fretta

*/

int insertBlocked(int *semAdd, struct pcb_t *p){
    void *tmp;
	struct semd_t *scan, *new_sem;
	int find = FALSE; // semAdd not found: FALSE
                      // semAdd found:     TRUE
	char *l = "cerco punto per inserire semaforo\n";
	char *m = "cerco semaforo\n";
	char *n = "semdFree vuota\n";
	char *i = "processo inserito\n";
	char *h = "trovato sem\n";
	char *d = "trovato sem e inserito processo\n";
	char *e = "sem non trovato e semdFree non vuota\n";
	char *f = "non trovato e semdFree vuota\n";
	char *g = "semaforo e processo inseriti\n";
	char *a = "sono nell'if di insertBlocked\n";
	char *c = "sono nel secondo if\n";
	char *b = "non ho fatto nulla\n";
	char *o = "dequeue semdFree\n";
	char *q = "verifico se semdFree è vuota\n";
	if (p != NULL) { 
			tprint(a);
	    	clist_foreach(scan, &aslh, s_link, tmp) { 
	    	tprint(m);
		    if (scan->s_semAdd == semAdd) {
   		        find = TRUE;
   		        tprint(h);
			    p->p_cursem->s_semAdd=semAdd;
			    break;
		    }
		}
		if (find) { // semaforo trovato
			insertProcQ(&scan->s_proc, p); 
			tprint(d);
			return FALSE;
		} 
		else { //semaforo non trovato
			tprint(q);
			if (semdFree.next != NULL) { //semdFree non vuota
				tprint(e);
				//scan = container_of(aslh.next, typeof(*scan), s_link); //inutile fa l'assegnamento nella foreach
				new_sem = container_of(semdFree.next, typeof(*new_sem), s_link);
				p->p_cursem = new_sem;
				new_sem->s_semAdd = semAdd;
				new_sem->s_proc.next = NULL;
				clist_dequeue(&semdFree);
				tprint(o);
				insertProcQ(new_sem->s_proc.next, p);
				tprint(i);
				if(aslh.next == NULL) {
					clist_enqueue(new_sem, &aslh, s_link);
					return FALSE;
				}
					clist_foreach(scan, &aslh, s_link, tmp){
						tprint(l);
						if(new_sem->s_semAdd < scan->s_semAdd){
							clist_foreach_add(new_sem, scan, &aslh, s_link, tmp);
							tprint(g);
							break;
						}
					}
						if (clist_foreach_all(scan, &aslh, s_link, tmp)) {
							clist_enqueue(new_sem, &aslh, s_link); 
							tprint(g);
						}
				return FALSE;
				}
			else { // caso lista semdFree vuota
				tprint(n);
				return TRUE; 
				}
			}	
	}
}



struct pcb_t *removeBlocked(int *semAdd){
	struct pcb_t *p;
	struct semd_t *scan;
	int trovato = 0;
	void *tmp;
	char *d = "sono in removeBlocked\n";
	char *a = "aslh vuota\n";
	char *b = "cerco il semaforo\n";
	char *c = "semaforo non trovato\n";
	tprint(d);
	if(aslh.next == NULL) {
		tprint(a);
		return NULL;
	}
	else{
		clist_foreach(scan, &aslh, s_link, tmp){
			tprint(b);
			if(scan->s_semAdd == semAdd){
				trovato = 1;
				break;
			}
			tprint(c);
		}
		if(trovato == 1){
			p = removeProcQ(&scan->s_proc);
			return p;
		}
		else return NULL;
	}

}



struct pcb_t *outBlocked(struct pcb_t *p); 



struct pcb_t *headBlocked(int *semAdd);
