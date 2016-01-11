#include "include/const.h"
#include "include/clist.h"
#include "include/pcb.h"
#include "include/asl.h"

HIDDEN struct clist aslh, semdFree;

/***************************************************************
*                    ACTIVE SEMAPHORE LIST                     *
***************************************************************/

void initASL(){
	static struct semd_t semdTable[MAXPROC];   
	int i;
	semdFree.next = &semdTable[MAXPROC-1].s_link;
	semdTable[MAXPROC-1].s_link.next = &semdTable[0].s_link;
	for(i = 0; i < (MAXPROC-1); i++){
		semdTable[i].s_link.next = &semdTable[i+1].s_link;
	}
}

