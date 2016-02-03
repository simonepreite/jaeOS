#ifndef _CLIST_H
#define _CLIST_H

//typedef unsigned int    size_t;

#define container_of(ptr, type, member) ({      \
		    const typeof( ((type *)0)->member ) *__mptr = (ptr);  \
		    (type *)( (char *)__mptr - offsetof(type,member) );	\
})

#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

/* struct clist definition. It is at the same time the type of the tail
	 pointer of the circular list and the type of field used to link the elements */
struct clist {
	struct clist *next;
};

/* constant used to initialize an empty list */
#define CLIST_INIT {NULL}

#define clist_enqueue(elem, clistp, member) ({\
	if((clistp)->next == NULL) {\
		(clistp)->next=&elem->member;\
		elem->list.next=&elem->member;\
	} else {\
		elem->list.next = (clistp)->next->next; \
    	(clistp)->next->next = &elem->list;  \
   		(clistp)->next = &elem->list;  \
	}\
})

#define clist_empty(clistx) (((clistx).next) == NULL)

#define clist_foreach(scan, clistp, member, tmp)  \
	if((clistp)->next != NULL)	\
	for (tmp = NULL,scan = container_of((clistp)->next->next, typeof(*scan), member);      \
		&scan->member != (tmp); scan = container_of(scan->member.next, typeof(*scan), member), tmp=(clistp)->next->next)

	/*
#define clist_foreach(scan, clistp, member, tmp)  \ //milos
  if((clistp)->next == NULL){ break; }  \
  int first_elem = 1;  \
  for (scan = container_of((clistp)->next->next, typeof(*scan), member), tmp = (clistp)->next->next;      \
    &scan->member != tmp || first_elem == 1; scan = container_of(scan->member.next, typeof(*scan), member), first_elem++)

	if((clistp)->next == NULL){ break; }	\
	else {	\
		for (scan = container_of((clistp)->next->next, typeof(*scan), member);      \
		&scan->member != (clistp)->next; \
		scan = container_of(scan->member.next, typeof(*scan), member)) \
			printf("== %p %d\n",scan,scan->value);	\
	}*/

#define clist_tail(elem, clistx, member)	\
	(clistx.next == NULL) ? NULL : container_of(clistx.next, typeof(*scan), member);

#define clist_head(elem, clistx, member)	\
	(clistx.next == NULL) ? NULL : container_of(clistx.next->next, typeof(*scan), member);

#define clist_push(elem, clistp, member) ({\
	if((clistp)->next == NULL) {\
		(clistp)->next=&elem->member;\
		elem->member.next=&elem->member;\
	} else { \
		elem->member.next = (clistp)->next->next;	\
		(clistp)->next->next = &elem->member;	\
	}	\
})

#define clist_pop(clistp) clist_dequeue(clistp)
#define clist_dequeue(clistp)	\
	if((clistp)->next != NULL){	\
		if((clistp)->next == (clistp)->next->next){	\
			(clistp)->next = NULL;	\
		} else {	\
			(clistp)->next->next = (clistp)->next->next->next;	\
		}	\
	}

#define clist_foreach_all(scan, clistp, member, tmp) &scan->member == (tmp)

#define clist_delete(elem, clistp, member)({\
int ret=1;\
struct clist *tmp1, *tmp2, *scan;\
if((clistp)->next!=NULL){\
	printf("sono nell if\n");\
	if(container_of((clistp)->next, typeof(*elem),member)==elem && (clistp)->next->next==(clistp)->next){\
		(clistp)->next=NULL;\
		ret=0;\
		printf("return is: %d\n",ret );\
}\
else{\
	printf("sono nell else\n");\
	for(tmp1=NULL, tmp2=(clistp)->next, scan=(clistp)->next->next; container_of(scan, typeof(*elem), member)!=elem && scan!=tmp1; tmp2=scan, scan=scan->next, tmp1=(clistp)->next) printf("sono nel for   ====  temp1: %p temp2: %p scan: %p elem: %p clistp->next: %p\n",tmp1, container_of(tmp2, typeof(*elem), member),container_of(scan, typeof(*elem), member),elem,(clistp)->next);\
  		printf("ho completato il for ==== temp1: %p temp2: %p scan: %p elem: %p clistp->next: %p\n",tmp1,container_of(tmp2, typeof(*elem), member),container_of(scan, typeof(*elem), member),elem,(clistp)->next);\
  		if(container_of(scan, typeof(*elem), member)==elem){\
  			printf("sono nel if dopo il for\n");\
			if (tmp2->next==(clistp)->next){\
				printf("sono nell'if caso eliminazione coda\n");\
  				tmp2->next=(clistp)->next->next;\
  				(clistp)->next=tmp2;\
  				scan->next=NULL;\
  				ret=0;\
  				printf("return is: %d\n",ret );\
    		}\
    		else{\
    			printf("sono nell else caso eliminazione elemento generico\n");\
				tmp2->next=scan->next;\
     			scan->next=NULL;\
     			ret=0;\
     			printf("return is: %d\n",ret );\
     		}\
  		}\
}\
}\
printf("return alla fine is: %d\n",ret );\
ret;\
})

#define clist_foreach_delete(scan, clistp, member, tmp)\
	for (tmp = &scan->member; scan->member.next != (tmp); scan = container_of(scan->member.next, typeof(*scan), member));\
		if((clistp)->next->next==&scan->member && (clistp)->next==&scan->member) (clistp)->next=NULL;\
		if((clistp)->next==(tmp)){\
			 scan->member.next=(clistp)->next->next;\
			 (clistp)->next=&scan->member;\
		}\
		else scan->member.next=scan->member.next->next;\
	scan=container_of((tmp), typeof(*scan), member);\

#define clist_foreach_add(elem, scan, clistp, member, tmp) \
		for (tmp = &scan->member; scan->member.next != (tmp); scan = container_of(scan->member.next, typeof(*scan), member));\
			elem->member.next=scan->member.next;\
			scan->member.next=&elem->member;\

#endif


