#ifndef PCB_H
#define PCB_H
#include "struct.h"
/* prototipi funzioni liste */

/* Lista */

void freePcb(pcb_t *p);
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

#endif
