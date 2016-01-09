#include "include/const.h"
#include "include/clist.h"
#include "include/pcb.h"
//#include "include/asl.h"
#include "include/struct.h"

/* Lista dei pcb inutilizzati */

HIDDEN struct clist *pcbFree_h = CLIST_INIT;


/*#################################################
  ###           GESTIONE LISTE PROCESSI         ###
  #################################################*/

//inserisce il pcb passato come parametro in coda alla lista

void freePcb(struct pcb_t *p){
	struct clist *t = pcbFree_h;
	clist_enqueue(p, t, p_list )
}

pcb_t *allocPcb(){
	pcb_t *pcb = NULL;
		if(pcbFree_h != NULL){
			pcb = container_of((pcbFree_h)->next, typeof(*pcb), p_list);
			clist_dequeue(pcb);
			pcb->p_parent=NULL;
			pcb->p_cursem=NULL;
			//pcb->p_s=0; da verificare;
			pcb->p_list=NULL;
			pcb->p_children=NULL;
			pcb->p_siblings=NULL;
		}
	return pcb;
}