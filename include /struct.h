#ifndef STRUTTURE
#define STRUTTURE

struct pcb_t {
	struct clist p_list; 
	struct semd_t *p_cursem; /*pointer to the sem_d on which process blocked*/
	state_t p_s; /*process state*/
	struct pcb_t *p_parent;
	struct clist p_children; 
	struct clist p_siblings; 
}pcb_t;


struct semd_t {
	int *s_semAdd; /*pointer to the semaphore*/
	struct clist s_link; /*ASL linked list*/
	struct clist s_proc; /*blocked process queue*/
}semd_t;


#endif