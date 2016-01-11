#include "include/const.h"
#include "include/clist.h"
#include "include/pcb.h"
#include "include/asl.h"

/* Lista dei pcb inutilizzati */

HIDDEN struct clist *pcbFree = NULL;

/***************************************************************
*           ALLOCATION AND DEALLOCATION PROCKBLKS'S            *
***************************************************************/

//inserisce il pcb passato come parametro in coda alla lista

void freePcb(struct pcb_t *p){
	struct clist *t = pcbFree;
	clist_enqueue(p, t, p_list);
}

/*
  toglie dalla lista dei processi liberi il primo e lo alloca
  inizializza la struttura in modo che non vi siano residui della
  vecchia allocazione                                           
*/

struct pcb_t *allocPcb(){
	struct pcb_t *pcb = NULL;
		if(pcbFree->next != NULL){
			pcb = container_of((pcbFree)->next->next, typeof(*pcb), p_list);
			clist_dequeue(pcbFree);
			pcb->p_parent = NULL;
			pcb->p_cursem = NULL;
			{
				pcb->p_s.a1 = pcb->p_s.a2 = pcb->p_s.a3 = pcb->p_s.a4 = 0; //da verificare la struttura state_p sta nella libreria uARMtypes.h;
				pcb->p_s.v1 = pcb->p_s.v2 = pcb->p_s.v3 = pcb->p_s.v4 = pcb->p_s.v5 = pcb->p_s.v6 = 0;
				pcb->p_s.sl = pcb->p_s.fp = pcb->p_s.ip = pcb->p_s.sp = pcb->p_s.lr = pcb->p_s.pc = pcb->p_s.cpsr = 0;
				pcb->p_s.CP15_Control = pcb->p_s.CP15_EntryHi = pcb->p_s.CP15_Cause = pcb->p_s.TOD_Hi = pcb->p_s.TOD_Low = 0;     
			}
			pcb->p_list.next = NULL;
			pcb->p_children.next = NULL;
			pcb->p_siblings.next = NULL;
		}
	return pcb;
}

// Inizializza la lista pcbFree, verrà chiamato solo una volta all'inizio

void initPcbs(){
	static struct pcb_t pcb_static[MAXPROC];
    int i;
	pcbFree->next = &pcb_static[MAXPROC-1].p_list;
	pcb_static[MAXPROC-1].p_list.next = &pcb_static[0].p_list;
	for(i = 0; i < (MAXPROC-1); i++){
		pcb_static[i].p_list.next = &pcb_static[i+1].p_list;
	}
}

/***************************************************************
*                   PROCESS QUEUE MAINTENANCE                  *
***************************************************************/

void insertProcQ(struct clist *q, struct pcb_t *p){
	clist_enqueue(p, q, p_list);
}

struct pcb_t *headProcQ(struct clist *q){
	struct clist temp = *q;
	struct pcb_t *pcb_temp; 
	pcb_temp = clist_head(pcb_temp, temp, p_list);
	return pcb_temp;
}

struct pcb_t *removeProcQ(struct clist *q){
	struct pcb_t *pcb_temp = NULL; 
		if(q->next != NULL){
			pcb_temp = container_of((q)->next->next, typeof(*pcb_temp), p_list);
			clist_dequeue(q);
		}
	return pcb_temp;
}

struct pcb_t *outProcQ(struct clist *q, struct pcb_t *p){
	if (clist_delete(p, q, p_list) == 0) return p;
	else return NULL;
}

/***************************************************************
*                   PROCESS TREE MAINTENANCE                   *
***************************************************************/

int emptyChild(struct pcb_t *p){
	if(p->p_children.next == NULL) return TRUE;
	else return FALSE;
}

void insertChild(struct pcb_t *parent, struct pcb_t *p){
	struct clist *q = &parent->p_children;
	clist_enqueue(p, q, p_children);
	p->p_parent = parent;
}


struct pcb_t *removeChild(struct pcb_t *p){
	struct pcb_t *pcb_temp = NULL; 
	struct clist *q = &p->p_children;
		if(p->p_children.next != NULL){
			pcb_temp = p;
			clist_dequeue(q);
		}
	return pcb_temp;
}

struct pcb_t *outChild(struct pcb_t *p){
	struct clist *q = &p->p_children;
	if(p->p_parent != NULL){
		clist_delete(p, q, p_children);
		return p;
	}
	else return NULL;
}
