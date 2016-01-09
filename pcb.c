#include "/usr/include/uarm/uARMconst.h"
#include "/usr/include/uarm/uARMtypes.h"
#include "include/const.h"
#include "include/clist.h"
#include "include/pcb.h"
//#include "include/asl.h"
#include "include/struct.h"

/* Lista dei pcb inutilizzati */

HIDDEN struct clist *pcbFree = NULL;


/*#################################################
  ###           GESTIONE LISTE PROCESSI         ###
  #################################################*/

//inserisce il pcb passato come parametro in coda alla lista

void freePcb(struct pcb_t *p){
	struct clist *t = pcbFree;
	clist_enqueue(p, t, p_list );
}

struct pcb_t *allocPcb(){
	struct pcb_t *pcb = NULL;
		if(pcbFree != NULL){
			pcb = container_of((pcbFree)->next, typeof(*pcb), p_list);
			clist_dequeue(pcbFree);
			pcb->p_parent=NULL;
			pcb->p_cursem=NULL;
			//pcb->p_s=0; da verificare;
			pcb->p_list.next=NULL;
			pcb->p_children.next=NULL;
			pcb->p_siblings.next=NULL;
		}
	return pcb;
}