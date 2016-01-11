#ifndef PCB_H
#define PCB_H
#include "/usr/include/uarm/uARMtypes.h"
#include "clist.h" 
/* struttura processi */

struct pcb_t{
	struct pcb_t *p_parent;
	struct semd_t *p_cursem; /*pointer to the sem_d on which process blocked*/
	state_t p_s; /*process state*/
	struct clist p_list; 
	struct clist p_children; 
	struct clist p_siblings; 
};

/* prototipi funzioni liste */

/* Lista */

EXTERN void freePcb(struct pcb_t *p);
EXTERN struct pcb_t *allocPcb();
EXTERN void initPcbs();

EXTERN void insertProcQ(struct clist *q, struct pcb_t *p);
EXTERN struct pcb_t *removeProcQ(struct clist *q);
EXTERN struct pcb_t *outProcQ(struct clist *q, struct pcb_t *p);
EXTERN struct pcb_t *headProcQ(struct clist *q);

/* Alberi */

EXTERN int emptyChild(struct pcb_t *p);
EXTERN void insertChild(struct pcb_t *parent, struct pcb_t *p);
EXTERN struct pcb_t *removeChild(struct pcb_t *p);
EXTERN struct pcb_t *outChild(struct pcb_t *p);

#endif
