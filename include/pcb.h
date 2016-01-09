#ifndef PCB_H
#define PCB_H

/* prototipi funzioni liste */

/* Lista */

 void freePcb(pcb_t *p);
EXTERN pcb_t *allocPcb(void);
EXTERN void initPcbs(void);

EXTERN void insertProcQ(struct clist *q, pcb_t *p);
EXTERN pcb_t *removeProcQ(struct clist *q);
EXTERN pcb_t *outProcQ(struct clist *q, pcb_t *p);
EXTERN pcb_t *headProcQ(struct clist *q);

/* Alberi */

EXTERN int emptyChild(pcb_t *p);
EXTERN void insertChild(pcb_t *parent, pcb_t *p);
EXTERN pcb_t *removeChild(pcb_t *p);
EXTERN pcb_t *outChild(pcb_t *p);



