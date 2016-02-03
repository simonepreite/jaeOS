#include "../include/const.h"
#include "../include/clist.h"
#include "../include/pcb.h"

/* Lista dei pcb inutilizzati */

HIDDEN struct clist pcbFree = CLIST_INIT;

/***************************************************************
*           ALLOCATION AND DEALLOCATION PROCKBLKS'S            *
***************************************************************/

//inserisce il pcb passato come parametro in coda alla lista

void freePcb(pcb_t *p){
	struct clist *t = &pcbFree;
	
	clist_enqueue(p, t, p_list); // incrementa la lista dei processi liberi perchè un processo non utilizza più lo spazio
}

/*

  toglie dalla lista dei processi liberi il primo e lo alloca
  inizializza la struttura in modo che non vi siano residui della
  vecchia allocazione    
                                         
*/

pcb_t *allocPcb(){
	pcb_t *pcb = NULL;

		if(pcbFree.next != NULL){ // controllo che la lista non sia vuota 
			pcb = container_of(pcbFree.next->next, typeof(*pcb), p_list); // punto pcb alla testa dei processi liberi
			clist_dequeue(&pcbFree); // decremento la lista dei processi liberi di uno
			pcb->p_parent = NULL;  // inizializzo la struttura per evitare resuidi di un vecchio utilizzo
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
	return pcb; // il puntatore resta NULL se non è possibile prelevare un processo
}

// Inizializza la lista pcbFree, verrà chiamato solo una volta all'inizio

void initPcbs(){
	static pcb_t pcb_static[MAXPROC]; // spazio dedicato ai processi necessario definirlo all'inizio a causa della mancanza di malloc
    int i;

	pcbFree.next = &pcb_static[MAXPROC-1].p_list; // linking della pcbFree all'array dei processi 
	pcb_static[MAXPROC-1].p_list.next = &pcb_static[0].p_list;
	for(i = 0; i < (MAXPROC-1); i++){
		pcb_static[i].p_list.next = &pcb_static[i+1].p_list;
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
	struct clist temp = *q;
	pcb_t *pcb_temp;

	pcb_temp = clist_head(pcb_temp, temp, p_list); 
	return pcb_temp;
}

//rimuove l'elemento in testa alla lista puntata in coda da q

 pcb_t *removeProcQ(struct clist *q){
	pcb_t *pcb_temp = NULL; 

		if(q->next != NULL){
			pcb_temp = container_of((q)->next->next, typeof(*pcb_temp), p_list);
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
	if(p->p_children.next == NULL) return TRUE;
	else return FALSE;
}

/*

inserisce il processo puntato da p in coda alla lista dei 
processi figli del processo puntato da parent

*/

void insertChild(pcb_t *parent, pcb_t *p){
	struct clist *q = &parent->p_children;

	clist_enqueue(p, q, p_siblings);
	p->p_parent = parent;
}

//decrementa la lista dei processi figli del processo puntato da p

pcb_t *removeChild(pcb_t *p){
	pcb_t *pcb_temp = NULL; 
	struct clist *q = &p->p_children;

		if(p->p_children.next != NULL){
			pcb_temp = container_of((q)->next->next, typeof(*pcb_temp), p_children); // DEVO DARGLI IL PROCESSO FIGLIO NON IL PADRE
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
	pcb_t *padre = p->p_parent;
	struct clist *figli = &padre->p_children;

	if(p->p_parent != NULL){
		clist_delete(p, figli, p_siblings);
		return p;
	}
	else return NULL;
}
