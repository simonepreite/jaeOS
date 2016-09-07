#include "const.h"
#include "clist.h"
#include "pcb.h"
#include "asl.h"

/* struttura semafori */

typedef struct semd_t{
	int *s_semAdd;
	struct clist s_link;
	struct clist s_procq;
}semd_t;


HIDDEN struct clist aslh, semdFree;


/***************************************************************
*                    ACTIVE SEMAPHORE LIST                     *
***************************************************************/

//Inizializza la lista semdFree, verrà chiamato solo una volta

void initASL(){
	// Initialization
	aslh = (struct clist)CLIST_INIT;
	semdFree = (struct clist)CLIST_INIT;
	static struct semd_t semdTable[MAXPROC];
	
	// Adding every sempahore to the semdFree list
	int i;
	for (i = 0; i < MAXPROC; i++) {
		struct semd_t *elem = &(semdTable[i]);
		clist_enqueue(elem, &semdFree, s_link);
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
/*
int insertBlocked(int *semAdd, pcb_t *p){
	semd_t *scan, *new_sem;

	if(aslh.next != NULL || semdFree.next != NULL){
	    clist_foreach(scan, &aslh, s_link, tmp) {
		    if (scan->s_semAdd == semAdd) {
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
*/
int insertBlocked(int *semAdd, struct pcb_t *p)
{
	struct semd_t *scan = NULL;
	void *tmp = NULL;
	
	// Searching active sempahore whose identifier is semAdd
	// This cycle won't be executed when the aslh list if empty
	clist_foreach(scan, &aslh, s_link, tmp) {
		if (semAdd == scan->s_semAdd) break;
	}
	
	int res = clist_foreach_all(scan, &aslh, s_link, tmp);
	
	// The searched active semaphore doesn't exist (it may not be active)
	if (res != 0) {
		// If the semdFree list is not empty, there are free descriptors
		// and a new semaphore can be allocated
		if (!clist_empty(semdFree)) {
			// Getting a pointer to the first free descriptor and making
			// it active by dequeuing it from the semdFree list
			struct semd_t *head = NULL;
			head = clist_head(head, semdFree, s_link);
			clist_dequeue(&semdFree);
			
			// Find the correct position where to insert the new semaphore
			// in the aslh list to keep the ascending order, based on semAdd
			// This cycle won't be executed when the aslh list is empty
			clist_foreach(scan, &aslh, s_link, tmp) {
				if (semAdd < scan->s_semAdd) {
					clist_foreach_add(head, scan, &aslh, s_link, tmp);
					break;
				}
			}
			if (clist_foreach_all(scan, &aslh, s_link, tmp))
				clist_enqueue(head, &aslh, s_link);
			
			// Setting pointers to initialize semaphore fields
			scan = head;
			head = NULL;
			
			scan->s_semAdd = semAdd;
			scan->s_procq = (struct clist)CLIST_INIT;
		}
		// A new semaphore needs to be allocated but no free descriptors are available
		else return TRUE;	
	}
	
	// Setting the blocked ProcBlk to the semaphore and adding it to the
	// process queue of the semaphore
	p->p_cursem = scan;
	insertProcQ(&(scan->s_procq), p);
	
	//scan = NULL;
	//tmp = NULL;
	
	return FALSE;
}
/*

rimuove il primo processo dalla coda dei processi bloccati nel
semaforo puntato da semAdd.
Se la coda dei processi bloccati risulta vuota dopo la rimozione
del processo il semaforo viene disattivato e torna alla lista dei
semafori liberi

*/
/*
pcb_t *removeBlocked(int *semAdd){
	pcb_t *p = NULL;
	semd_t *scan = NULL;

	if(aslh.next == NULL) {
		return NULL;
	}
	else{
		clist_foreach(scan, &aslh, s_link, tmp){
			if( scan->s_semAdd <= semAdd){
				if(scan->s_semAdd == semAdd){
					p = removeProcQ(&scan->s_proc);
					if(scan->s_proc.next==NULL){
						clist_foreach_delete(scan, &aslh, s_link, tmp);
						clist_enqueue(scan, &semdFree, s_link);
					}
					return p;
				}
			}
		}
	return p;
	}
}*/
struct pcb_t* removeBlocked(int *semAdd)
{
	struct semd_t *scan = NULL;
	void *tmp = NULL;
	
	// Searching active sempahore whose identifier is semAdd
	// This cycle won't be executed when the aslh list if empty
	clist_foreach(scan, &aslh, s_link, tmp) {
		if (semAdd == scan->s_semAdd) break;
	}
	
	int res = clist_foreach_all(scan, &aslh, s_link, tmp);
	
	// Return NULL if the searched semaphore is not active (it may not be active)
	if (res != 0) return NULL;
	
	// Remove the first blocked ProcBlk from the process queue of the sempahore
	// whose identifier is semAdd and get a pointer to that ProcBlk
	struct pcb_t *proc = removeProcQ(&(scan->s_procq));
	proc->p_cursem = NULL;
	proc->p_list = (struct clist)CLIST_INIT;
	
	// Checking if the process queue of the semaphore gets empty after removing
	// a ProcBlk so that the entire sempahore can be deallocate and returned to
	// the semdFree list
	if (clist_empty(scan->s_procq)) {
		if (clist_delete(scan, &aslh, s_link) == 0) {
			clist_enqueue(scan, &semdFree, s_link);
		}
	}
	
	return proc;
}

/*

rimuove dalla coda dei processi bloccati nel semaforo puntato
da p->p_cursem il processo p.
Se la coda dei processi bloccati risulta vuota dopo la rimozione
del processo il semaforo viene disattivato e torna alla lista dei
semafori liberi

*/
/*
pcb_t *outBlocked(pcb_t *p){
	 pcb_t *out = NULL;
	 struct clist *q = &p->p_cursem->s_proc;

	if(q->next!=NULL){
			out = outProcQ(&p->p_cursem->s_proc, p);
			if(p->p_cursem->s_proc.next==NULL){
				clist_delete(p->p_cursem, &aslh, s_link);
				clist_enqueue(p->p_cursem, &semdFree, s_link);
			}
	}
	return out;
}*/

struct pcb_t* outBlocked(struct pcb_t *p)
{
	if (p->p_cursem == NULL) return NULL;
	
	// Getting a pointer to the process queue of the semaphore where
	// the ProcBlk pointed to by p is blocked against
	struct clist *procscan = &(p->p_cursem->s_procq);
	
	// Remove the ProcBlk from the process queue of the semphore
	struct pcb_t *proc = outProcQ(procscan, p);

	// Dealloc semaphore description if last blocked ProcBlk is removed
	if (clist_empty(p->p_cursem->s_procq)) {
		clist_delete(p->p_cursem, &aslh, s_link);
		clist_enqueue(p->p_cursem, &semdFree, s_link);
	}

	return proc;
}

//ritorna il puntatore alla testa dei processi bloccati in semAdd
/*
pcb_t *headBlocked(int *semAdd){
	pcb_t *p = NULL;
	semd_t *scan;

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
}*/

struct pcb_t* headBlocked(int *semAdd)
{
	struct semd_t *scan = NULL;
	void *tmp = NULL;
	
	// Searching active sempahore whose identifier is semAdd
	// This cycle won't be executed when the aslh list if empty
	clist_foreach(scan, &aslh, s_link, tmp) {
		if (semAdd == scan->s_semAdd) break;
	}
	
	int res = clist_foreach_all(scan, &aslh, s_link, tmp);
	
	// Return NULL if the searched semaphore is not found (it may not be active)
	if (res != 0) return NULL;
	else {
		// Return NULL is the semaphore is active but it has not processes
		// in its process queue
		if (clist_empty(scan->s_procq)) return NULL;
		
		// Getting a pointer to the first blocked ProcBlk of the semaphore
		struct pcb_t *head = headProcQ(&(scan->s_procq));
		return head;
	}
}


/***************************************************************
*                      AUXILIARY FUNCTION                      *
***************************************************************/

/*

prende un semaphoro dalla lista dei semdFree e lo attiva
ammesso che la lista dei semafori liberi non sia vuota

*/
/*
semd_t *new_semaphore(semd_t *new_sem, pcb_t *p, int *semAdd){
	new_sem = NULL;
	if(!clist_empty(semdFree)){
		new_sem = clist_head( new_sem, semdFree, s_link);
		p->p_cursem = new_sem;
		new_sem->s_semAdd = semAdd;
		new_sem->s_proc.next = NULL;
		clist_dequeue(&semdFree); // decremento della lista dei semafori liberi
		insertProcQ(&new_sem->s_proc, p);
	}
	return new_sem;
}
*/
void updateSemaphoreValue(struct semd_t *sem, int delta) {
	if (!sem) PANIC();

	*(sem->s_semAdd) += delta;
}
