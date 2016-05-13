#ifndef PCB_H
#define PCB_H
#include "uARMtypes.h"
#include "types.h"
#include "clist.h" 

/* struttura processi 

typedef struct pcb_t{
	struct pcb_t *p_parent;
	struct semd_t *p_cursem; //pointer to the sem_d on which process blocked
	state_t p_s; //process state
	struct clist p_list; 
	struct clist p_children; 
	struct clist p_siblings; 
}pcb_t;
*/

/* prototipi funzioni liste */

/* Lista */

EXTERN void freePcb(pcb_t *p);
EXTERN pcb_t *allocPcb();
EXTERN void initPcbs();

EXTERN void insertProcQ(struct clist *q,  pcb_t *p);
EXTERN  pcb_t *removeProcQ(struct clist *q);
EXTERN  pcb_t *outProcQ(struct clist *q,  pcb_t *p);
EXTERN  pcb_t *headProcQ(struct clist *q);

/* Alberi */

EXTERN int emptyChild( pcb_t *p);
EXTERN void insertChild(pcb_t *parent, pcb_t *p);
EXTERN pcb_t *removeChild(pcb_t *p);
EXTERN pcb_t *outChild(pcb_t *p);

/* funzioni ausiliarie */

EXTERN void init_proc(pcb_t *pcb);

#endif
