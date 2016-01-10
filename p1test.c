/*****************************************************************************
 * p1test.c Year 2015 v.0.1 January, 08 2016                                 *
 * Copyright 2004, 2005 Michael Goldweber, Davide Brini.                     *
 * Modified 2010-2016 Renzo Davoli, Marco Melletti                           *
 *                                                                           *
 * This file is part of jaeos16. (derived from Kaya)                         *
 *                                                                           *
 * Jaeos16 is free software; you can redistribute it and/or modify it under  *
 * the terms of the GNU General Public License as published by the Free      *
 * Software Foundation; either version 2 of the License, or (at your option) *
 * any later version.                                                        *
 * This program is distributed in the hope that it will be useful, but       *
 * WITHOUT ANY WARRANTY; without even the implied warranty of                *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General *
 * Public License for more details.                                          *
 * You should have received a copy of the GNU General Public License along   *
 * with this program; if not, write to the Free Software Foundation, Inc.,   *
 * 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.                  *
 *****************************************************************************/

/*********************************P1TEST.C*******************************
 *
 *	Test program for the modules ASL and pcbQueues (phase 1).
 *
 *	Produces progress messages on terminal 0 
 * 	Error messages will also appear on terminal 0 
 *	Aborts as soon as an error is detected.
 */
#include "/usr/include/uarm/uARMconst.h"
#include "/usr/include/uarm/uARMtypes.h"
#include "/usr/include/uarm/libuarm.h"

#include "include/const.h"
#include "include/clist.h"
#include "include/pcb.h"
#include "include/asl.h"


#define	MAXSEM	MAXPROC

int sem[MAXSEM];
int onesem;
struct pcb_t	*procp[MAXPROC], *p, *q, *firstproc, *lastproc, *midproc;
struct clist qa;

/* This function places the specified character string in okbuf and
 *      causes the string to be written out to terminal0 */
void addokbuf(char *strp) {
	tprint(strp);
}


/* This function places the specified character string in errbuf and
 *      causes the string to be written out to terminal0.  After this is done
 *      the system shuts down with a panic message */
void adderrbuf(char *strp) {
	tprint(strp);
	PANIC();
}

int main() {
	int i;
	struct clist empty=CLIST_INIT;

	initPcbs();
	addokbuf("Initialized process control blocks   \n");

	/* Check allocPcb */
	for (i = 0; i < MAXPROC; i++) {
		char *a="ciao!\n";
		tprint(a);
		if ((procp[i] = allocPcb()) == NULL){
			adderrbuf("allocPcb(): unexpected NULL   ");
			char *b="if\n";
			tprint(b);
		}
	}
	char *b="if\n";
	tprint(b);
	if (allocPcb() != NULL) {
		adderrbuf("allocPcb(): allocated more than MAXPROC entries   ");
	}
	addokbuf("allocPcb ok   \n");

	return 0;
}