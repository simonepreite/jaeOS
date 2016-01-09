#ifndef STRUTTURE
#define STRUTTURE
/************************* CPU state type ***********************************/

typedef struct{
    unsigned int a1;    //r0
    unsigned int a2;    //r1
    unsigned int a3;    //r2
    unsigned int a4;    //r3
    unsigned int v1;    //r4
    unsigned int v2;    //r5
    unsigned int v3;    //r6
    unsigned int v4;    //r7
    unsigned int v5;    //r8
    unsigned int v6;    //r9
    unsigned int sl;    //r10
    unsigned int fp;    //r11
    unsigned int ip;    //r12
    unsigned int sp;    //r13
    unsigned int lr;    //r14
    unsigned int pc;    //r15
    unsigned int cpsr;
    unsigned int CP15_Control;
    unsigned int CP15_EntryHi;
    unsigned int CP15_Cause;
    unsigned int TOD_Hi;
    unsigned int TOD_Low;
}state_t;

struct pcb_t{
	struct pcb_t *p_parent;
	struct semd_t *p_cursem; /*pointer to the sem_d on which process blocked*/
	state_t p_s; /*process state*/
	struct clist p_list; 
	struct clist p_children; 
	struct clist p_siblings; 
}pcb_t;


struct semd_t{
	int *s_semAdd; /*pointer to the semaphore*/
	struct clist s_link; /*ASL linked list*/
	struct clist s_proc; /*blocked process queue*/
}semd_t;


#endif