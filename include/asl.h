//#ifndef ASL_H
//#define ASL_H

/* struttura semafori */

struct semd_t{
	int *s_semAdd; /*pointer to the semaphore*/
	struct clist s_link; /*ASL linked list*/
	struct clist s_proc; /*blocked process queue*/
};


/* prototipi funzioni semafori */

void initASL(void);
int insertBlocked(int *semAdd, struct pcb_t *p); /*Inserisce il ProcBlk puntato da p*/
struct pcb_t *removeBlocked(int *semAdd); /* rimuove il primo ProcBlk, se trovato */
struct pcb_t *outBlocked(struct pcb_t *p); /* Rimuove il ProcBlk puntato da p */
struct pcb_t *headBlocked(int *semAdd); /*ritorna un puntatore al ProcBlk */

//#endif

