#ifndef PCB_H
#define PCB_H
#include "struct.h"
/* prototipi funzioni liste */

/* Lista */

void freePcb(pcb_t *p);
pcb_t *allocPcb(void);
void initPcbs(void);

void insertProcQ(pcb_t *q, pcb_t *p);
pcb_t *removeProcQ(pcb_t *q);
pcb_t *outProcQ(pcb_t *q, pcb_t *p);
pcb_t *headProcQ(struct clist *q);

/* Alberi */

int emptyChild(pcb_t *p);
void insertChild(pcb_t *parent, pcb_t *p);
pcb_t *removeChild(pcb_t *p);
pcb_t *outChild(pcb_t *p);

#endif
