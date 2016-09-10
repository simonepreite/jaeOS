/*
 *	ASL.H
 *	Active Semaphore List Header File
 *
 *	Gruppo 28:
 *	Del Vecchio Matteo
 *	Preite Simone
 */

#ifndef ASL_H
#define ASL_H

/* prototipi funzioni semafori */

EXTERN void initASL();
EXTERN int insertBlocked(int *semAdd, pcb_t *p); /*Inserisce il ProcBlk puntato da p*/
EXTERN pcb_t *removeBlocked(int *semAdd); /* rimuove il primo ProcBlk, se trovato */
EXTERN pcb_t *outBlocked(pcb_t *p); /* Rimuove il ProcBlk puntato da p */
EXTERN pcb_t *headBlocked(int *semAdd); /*ritorna un puntatore al ProcBlk */

/* funzioni ausiliarie */

EXTERN struct semd_t *new_semaphore(struct semd_t *new_sem, pcb_t *p, int *semAdd); /* attiva un semaforo se è possibilie */
EXTERN void updateSemaphoreValue(struct semd_t *sem, int delta);	/* aggiorna il valore del semaforo sem aggiungendo delta (usare valori negativi per diminuire il valore) */

#endif

