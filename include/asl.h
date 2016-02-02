#ifndef ASL_H
#define ASL_H

/* struttura semafori */

typedef struct semd_t{
	int *s_semAdd; /*pointer to the semaphore*/
	struct clist s_link; /*ASL linked list*/
	struct clist s_proc; /*blocked process queue*/
}semd_t;


/* prototipi funzioni semafori */

EXTERN void initASL();
EXTERN int insertBlocked(int *semAdd, struct pcb_t *p); /*Inserisce il ProcBlk puntato da p*/
EXTERN pcb_t *removeBlocked(int *semAdd); /* rimuove il primo ProcBlk, se trovato */
EXTERN pcb_t *outBlocked(pcb_t *p); /* Rimuove il ProcBlk puntato da p */
EXTERN pcb_t *headBlocked(int *semAdd); /*ritorna un puntatore al ProcBlk */

#endif

