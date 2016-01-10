#include "include/const.h"
#include "include/clist.h"
#include "include/pcb.h"
//#include "include/asl.h"

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
			clist_enqueue(pcb, pcbFree, p_list);
			pcb->p_parent=NULL;
			pcb->p_cursem=NULL;
			//pcb->p_s=0; da verificare;
			pcb->p_list.next=NULL;
			pcb->p_children.next=NULL;
			pcb->p_siblings.next=NULL;
		}
	return pcb;
}

// Inizializza la lista pcbFree, verrà chiamato solo una volta all'inizio

void initPcbs(){
	static struct pcb_t pcb_static[MAXPROC];
    int i;
	pcbFree = &pcb_static[MAXPROC].p_list;
	for(i = 0; i < (MAXPROC-1); i++){
		pcb_static[i].p_list.next = &pcb_static[i+1].p_list;
	}
	pcb_static[MAXPROC].p_list.next= &pcb_static[0].p_list;
}