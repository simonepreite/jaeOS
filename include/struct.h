#ifndef STRUTTURE
#define STRUTTURE
#include <uARMtypes.h>

struct pcb_t{
	struct pcb_t *p_parent;
	struct semd_t *p_cursem; /*pointer to the sem_d on which process blocked*/
	state_t p_s; /*process state*/
	struct clist p_list; 
	struct clist p_children; 
	struct clist p_siblings; 
};


struct semd_t{
	int *s_semAdd; /*pointer to the semaphore*/
	struct clist s_link; /*ASL linked list*/
	struct clist s_proc; /*blocked process queue*/
};


#endif