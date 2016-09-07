#include "const.h"
#include "clist.h"
#include "pcb.h"

/* Lista dei pcb inutilizzati */

HIDDEN struct clist pcbFree;

/***************************************************************
*           ALLOCATION AND DEALLOCATION PROCKBLKS'S            *
***************************************************************/

//inserisce il pcb passato come parametro in coda alla lista
/*
void freePcb(pcb_t *p){
	clist_enqueue(p, &(pcbFree), p_list);
}*/
void freePcb(struct pcb_t *p)
{
	// If the ProcBlk pointed to by p is not NULL, it gets deallocated
	// by adding it to the pcbFree list. Fields don't get changed because
	// they will be set next time the ProcBlk will be allocated and reused.
	if (p != NULL) clist_enqueue(p, &pcbFree, p_list);
}

/*

  toglie dalla lista dei processi liberi il primo e lo alloca
  inizializza la struttura in modo che non vi siano residui della
  vecchia allocazione

*/
/*
pcb_t *allocPcb(){
	pcb_t *p = NULL;

		if(!(clist_empty(pcbFree))){
			p = clist_head(p, pcbFree, p_list);
			clist_dequeue(&pcbFree);
			init_proc(p);
		}
	return p; // il puntatore resta NULL se non è possibile prelevare un processo
}*/

struct pcb_t* allocPcb()
{
	struct pcb_t *head = NULL;
	
	// If the pcbFree list is not empty
	// a new ProcBlk can be allocated
	if (!clist_empty(pcbFree)) {
		// Getting a pointer to the first free ProcBlk
		head = clist_head(head, pcbFree, p_list);
		clist_dequeue(&pcbFree);	// and removing it from the pcbFree list
		
		// Initializing ProcBlk fields
		head->p_list = (struct clist)CLIST_INIT;
		head->p_children = (struct clist)CLIST_INIT;
		head->p_siblings = (struct clist)CLIST_INIT;
		head->p_parent = NULL;
		head->p_cursem = NULL;
		
		head->pid = (pid_t)&head;
		head->kernel_mode = 0;
		head->global_time = 0;
		head->lastTimeSlice = 0;
		head->waitingResCount = 0;
		head->tags = 0;
		int i;
		for(i=0; i<EXCP_COUNT;i++)
			init_state_t(head->excp_state_vector[i]);
		
		head->p_s.a1 = 0;
		head->p_s.a2 = 0;
    	head->p_s.a3 = 0;
    	head->p_s.a4 = 0;
    	head->p_s.v1 = 0;
    	head->p_s.v2 = 0;
    	head->p_s.v3 = 0;
    	head->p_s.v4 = 0;
    	head->p_s.v5 = 0;
    	head->p_s.v6 = 0;
    	head->p_s.sl = 0;
    	head->p_s.fp = 0;
    	head->p_s.ip = 0;
  		head->p_s.sp = 0;
    	head->p_s.lr = 0;
    	head->p_s.pc = 0;
    	head->p_s.cpsr = 0;
    	head->p_s.CP15_Control = 0;
    	head->p_s.CP15_EntryHi = 0;
    	head->p_s.CP15_Cause = 0;
    	head->p_s.TOD_Hi = 0;
    	head->p_s.TOD_Low = 0;
	}
	
	// Returning pointer to the allocated ProcBlk
	return head;
}

// Inizializza la lista pcbFree, verrà chiamato solo una volta all'inizio
/*
void initPcbs(){
	static pcb_t pcb_static[MAXPROC];
  int i;

	for(i = 0; i < (MAXPROC); i++){
		pcb_t *q = &pcb_static[i];
		clist_enqueue(q, &pcbFree, p_list);
	}
}*/
void initPcbs(void)
{
	// Initialization
	pcbFree = (struct clist)CLIST_INIT;
	static struct pcb_t procArray[MAXPROC];
	
	// Adding every ProcBlk to the pcbFree list
	int i;
	for (i = 0; i < MAXPROC; i++) {
		struct pcb_t *elem = &(procArray[i]);
		clist_enqueue(elem, &pcbFree, p_list);
	}
}

/***************************************************************
*                   PROCESS QUEUE MAINTENANCE                  *
***************************************************************/

//inserisce il processo puntato da p nella lista puntata in coda da q
/*
void insertProcQ(struct clist *q, pcb_t *p){
	clist_enqueue(p, q, p_list);
}*/
void insertProcQ(struct clist *q, struct pcb_t *p)
{
	// If the ProcBlk pointed to by p is not NULL, it will be added to
	// the process queue whose tail pointer is q. If the process queue
	// pointed to by q is NULL, it will be set up when adding the
	// first ProcBlk.
	if (p != NULL) clist_enqueue(p, q, p_list);
}

/*

ritorna il puntatore alla testa della lista puntata in coda da q, se
la lista risulta vuota ritorna NULL, non elimina alcun elemento

*/
/*
pcb_t *headProcQ(struct clist *q){
	pcb_t *pcb_temp = clist_head(pcb_temp, (*q), p_list); ;
	return pcb_temp;
}*/
struct pcb_t* headProcQ(struct clist *q)
{
	// Getting a pointer to the first ProcBlk in the process queue
	// whose tail pointer is q and returning it without removing it
	struct pcb_t *first;
	first = clist_head(first, (*q), p_list);
	return first;
}

//rimuove l'elemento in testa alla lista puntata in coda da q
/*
 pcb_t *removeProcQ(struct clist *q){
	pcb_t *pcb_temp = NULL;

		if(q->next != NULL){
			pcb_temp = clist_head(pcb_temp, (*q), p_list);
			clist_dequeue(q);
		}
	return pcb_temp;
}*/
struct pcb_t* removeProcQ(struct clist *q)
{
	if (q->next == NULL) return NULL;
	
	// Getting a pointer to the first ProcBlk in the process queue
	// whose tail pointer is q and returning it after removing it
	// from the process queue
	struct pcb_t *head;
	head = clist_head(head, (*q), p_list);
	
	clist_dequeue(q);
	return head;
}

//rimuove il processo puntato da p nella lista puntata in coda da q
/*
pcb_t *outProcQ(struct clist *q, pcb_t *p){
	if (clist_delete(p, q, p_list) == 0) return p;
	else return NULL;
}*/
struct pcb_t* outProcQ(struct clist *q, struct pcb_t *p)
{
	if (q->next == NULL || p == NULL) return NULL;
	
	// Searching the ProcBlk pointed to by p in the process queue
	// whose tail pointer is q, removing it from the process queue
	// and returning it.
	if (clist_delete(p, q, p_list) != 0) return NULL;
	else return p;
}

/***************************************************************
*                   PROCESS TREE MAINTENANCE                   *
***************************************************************/

/*

ritorna TRUE se il processo puntato da p ha figli altrimenti
FALSE

*/
/*
int emptyChild(pcb_t *p){
	if(clist_empty(p->p_children)) return TRUE;
	else return FALSE;
}*/
int emptyChild(struct pcb_t *p)
{
	if (p != NULL && clist_empty(p->p_children)) return TRUE;
	else return FALSE;
}

/*

inserisce il processo puntato da p in coda alla lista dei
processi figli del processo puntato da parent

*/
/*
void insertChild(pcb_t *parent, pcb_t *p){
	clist_enqueue(p, &(parent->p_children), p_siblings);
	p->p_parent = parent;
}*/
void insertChild(struct pcb_t *parent, struct pcb_t *p)
{
	if (parent != NULL && p != NULL) {
		p->p_parent = parent;	// Settings parent pointer of p
		
		// Adding the ProcBlk pointed to by p to the list of
		// children of the ProcBlk pointed to by parent
		clist_enqueue(p, &(parent->p_children), p_siblings);
	}
}

//decrementa la lista dei processi figli del processo puntato da p
/*
pcb_t *removeChild(pcb_t *p){
	pcb_t *pcb_temp = NULL;
	struct clist *q = &p->p_children;

		if(p->p_children.next != NULL){
			pcb_temp = clist_head(pcb_temp, (*q), p_children);
			pcb_temp->p_parent = NULL;
			clist_dequeue(q);
		}
	return pcb_temp;
}*/
struct pcb_t* removeChild(struct pcb_t *p)
{
	if (p == NULL || emptyChild(p)) return NULL;
	
	// Getting a pointer to the first child of ProcBlk pointed
	// to by p, removing it from the list of children and returning it
	struct pcb_t *head;
	head = clist_head(head, p->p_children, p_siblings);
	clist_dequeue(&(p->p_children));
	head->p_parent = NULL;
	return head;
}

/*

toglie il processo puntato da p dalla lista dei processi del suo
parent e ritorna il puntatore del processo eliminato,
se il processo p non ha un parent torna NULL

*/
/*
pcb_t *outChild(pcb_t *p){
	if(p->p_parent != NULL){
		if(clist_delete(p, &(p->p_parent->p_children) , p_siblings)==0) return p;
	}
	else return NULL;
}*/
struct pcb_t* outChild(struct pcb_t *p)
{
	if (p == NULL || p->p_parent == NULL) return NULL;
	
	// Getting the tail pointer to the list of children
	struct clist *children = &(p->p_parent->p_children);
	
	// Searching the ProcBlk pointed to by p, removing it from
	// the list of children and returning it
	if (clist_delete(p, children, p_siblings) != 0) return NULL;
	else {
		p->p_parent = NULL;
		return p;
	}
}

/***************************************************************
*                      AUXILIARY FUNCTION                      *
***************************************************************/

void init_proc(pcb_t *pcb){

	int i = 0;

	pcb->p_parent = NULL;  // inizializzo la struttura per evitare resuidi di un vecchio utilizzo
	pcb->p_cursem = NULL;
	pcb->pid = (pid_t)&pcb;
	init_state_t(pcb->p_s);
	for(i=0; i<EXCP_COUNT;i++)
		init_state_t(pcb->excp_state_vector[i]);
	pcb->p_list.next = NULL;
	pcb->p_children.next = NULL;
	pcb->p_siblings.next = NULL;
	pcb->kernel_mode = 0;
	pcb->global_time = 0;
	pcb->lastTimeSlice = 0;
	pcb->waitingResCount = 0;
	pcb->tags = 0;
}
