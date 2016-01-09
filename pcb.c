#include "include/const.h"
#include "include/clist.h"
#include "include/pcb.h"
#include "include/asl.h"
/* Lista dei pcb inutilizzati */

HIDDEN struct clist *pcbFree_h;

/*#################################################
  ###           GESTIONE LISTE PROCESSI         ###
  #################################################*/

//inserisce il pcb passato come parametro in coda alla lista

void freePcb(struct pcb_t *p){
	struct clist *t = pcbFree_h;
	clist_enqueue(p, &t, p_list )
}


