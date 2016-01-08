#ifndef ASL_H
#define ASL_H

/* prototipi funzioni semafori */

EXTERN void initASL(void);
EXTERN int insertBlocked(int *semAdd, pcb_t *p); /*Inserisce il ProcBlk puntato da p*/
EXTERN pcb_t *removeBlocked(int *semAdd); /* rimuove il primo ProcBlk, se trovato */
EXTERN pcb_t *outBlocked(pcb_t *p); /* Rimuove il ProcBlk puntato da p */
EXTERN pcb_t *headBlocked(int *semAdd); /*ritorna un puntatore al ProcBlk */

#endif

