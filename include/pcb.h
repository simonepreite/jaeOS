#ifndef PCB_H
#define PCB_H
#include "struct.h"
/* prototipi funzioni liste */

/* Lista */

/*EXTERN void freePcb(struct pcb_t *p);
EXTERN struct pcb_t *allocPcb(void);
EXTERN void initPcbs(void);

EXTERN void insertProcQ(struct clist *q, struct pcb_t *p);
EXTERN struct pcb_t *removeProcQ(struct clist *q);
EXTERN struct pcb_t *outProcQ(struct clist *q, struct pcb_t *p);
EXTERN struct pcb_t *headProcQ(struct clist *q);

/* Alberi */

/*EXTERN int emptyChild(struct pcb_t *p);
EXTERN void insertChild(struct pcb_t *parent, struct pcb_t *p);
EXTERN struct pcb_t *removeChild(struct pcb_t *p);
EXTERN struct pcb_t *outChild(struct pcb_t *p);*/

EXTERN void freePcb(pcb_t *p);
EXTERN pcb_t *allocPcb(void);
EXTERN void initPcbs(void);

EXTERN void insertProcQ(pcb_t *q, pcb_t *p);
EXTERN pcb_t *removeProcQ(pcb_t *q);
EXTERN pcb_t *outProcQ(pcb_t *q, pcb_t *p);
EXTERN pcb_t *headProcQ(struct clist *q);

/* Funzioni per alberi */

EXTERN int emptyChild(pcb_t *p);
EXTERN void insertChild(pcb_t *parent, pcb_t *p);
EXTERN pcb_t *removeChild(pcb_t *p);
EXTERN pcb_t *outChild(pcb_t *p);

#endif
