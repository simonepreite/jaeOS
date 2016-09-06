#include "const.h"
#include "clist.h"
#include "pcb.h"

/* Lista dei pcb inutilizzati */

HIDDEN struct clist pcbFree = CLIST_INIT;

/***************************************************************
*           ALLOCATION AND DEALLOCATION PROCKBLKS'S            *
***************************************************************/

//inserisce il pcb passato come parametro in coda alla lista

void freePcb(pcb_t *p){
	clist_enqueue(p, &(pcbFree), p_list);
}

/*

  toglie dalla lista dei processi liberi il primo e lo alloca
  inizializza la struttura in modo che non vi siano residui della
  vecchia allocazione

*/

pcb_t *allocPcb(){
	pcb_t *p = NULL;

		if(!(clist_empty(pcbFree))){
			p = clist_head(p, pcbFree, p_list);
			clist_dequeue(&pcbFree);
			init_proc(p);
		}
	return p; // il puntatore resta NULL se non è possibile prelevare un processo
}

// Inizializza la lista pcbFree, verrà chiamato solo una volta all'inizio

void initPcbs(){
	static pcb_t pcb_static[MAXPROC];
  int i;

	for(i = 0; i < (MAXPROC); i++){
		pcb_t *q = &pcb_static[i];
		clist_enqueue(q, &pcbFree, p_list);
	}
}

/***************************************************************
*                   PROCESS QUEUE MAINTENANCE                  *
***************************************************************/

//inserisce il processo puntato da p nella lista puntata in coda da q

void insertProcQ(struct clist *q, pcb_t *p){
	clist_enqueue(p, q, p_list);
}

/*

ritorna il puntatore alla testa della lista puntata in coda da q, se
la lista risulta vuota ritorna NULL, non elimina alcun elemento

*/

pcb_t *headProcQ(struct clist *q){
	pcb_t *pcb_temp = clist_head(pcb_temp, (*q), p_list); ;
	return pcb_temp;
}

//rimuove l'elemento in testa alla lista puntata in coda da q

 pcb_t *removeProcQ(struct clist *q){
	pcb_t *pcb_temp = NULL;

		if(q->next != NULL){
			pcb_temp = clist_head(pcb_temp, (*q), p_list);
			clist_dequeue(q);
		}
	return pcb_temp;
}

//rimuove il processo puntato da p nella lista puntata in coda da q

pcb_t *outProcQ(struct clist *q, pcb_t *p){
	if (clist_delete(p, q, p_list) == 0) return p;
	else return NULL;
}

/***************************************************************
*                   PROCESS TREE MAINTENANCE                   *
***************************************************************/

/*

ritorna TRUE se il processo puntato da p ha figli altrimenti
FALSE

*/

int emptyChild(pcb_t *p){
	if(clist_empty(p->p_children)) return TRUE;
	else return FALSE;
}

/*

inserisce il processo puntato da p in coda alla lista dei
processi figli del processo puntato da parent

*/

void insertChild(pcb_t *parent, pcb_t *p){
	clist_enqueue(p, &(parent->p_children), p_siblings);
	p->p_parent = parent;
}

//decrementa la lista dei processi figli del processo puntato da p

pcb_t *removeChild(pcb_t *p){
	pcb_t *pcb_temp = NULL;
	struct clist *q = &p->p_children;

		if(p->p_children.next != NULL){
			pcb_temp = clist_head(pcb_temp, (*q), p_children);
			clist_dequeue(q);
		}
	return pcb_temp;
}

/*

toglie il processo puntato da p dalla lista dei processi del suo
parent e ritorna il puntatore del processo eliminato,
se il processo p non ha un parent torna NULL

*/

pcb_t *outChild(pcb_t *p){
	if(p->p_parent != NULL){
		if(clist_delete(p, &(p->p_parent->p_children) , p_siblings)==0) return p;
	}
	else return NULL;
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
