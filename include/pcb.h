//#ifndef PCB_H
//#define PCB_H
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

void freePcb(struct pcb_t *p);
struct pcb_t *allocPcb();
void initPcbs();

void insertProcQ(struct clist *q, struct pcb_t *p);
struct pcb_t *removeProcQ(struct clist *q);
struct pcb_t *outProcQ(struct clist *q, struct pcb_t *p);
struct pcb_t *headProcQ(struct clist *q);

/* Alberi */

int emptyChild(struct pcb_t *p);
void insertChild(struct pcb_t *parent, struct pcb_t *p);
struct pcb_t *removeChild(struct pcb_t *p);
struct pcb_t *outChild(struct pcb_t *p);

//#endif
