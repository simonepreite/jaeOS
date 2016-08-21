#compilatore
CC = arm-none-eabi-gcc
#flag compilatore
FLAG_CC = -mcpu=arm7tdmi -I $(INCL_UARM) -I $(INCL_DIR) -c -o
#linker
UL = arm-none-eabi-ld
#flag linker
FLAG_UL = -T
#converitore per uARM
UC = elf2uarm
#flag converter uARM
FLAG_UC = -k
#percorso librerie uARM
INCL_UARM = /usr/include/uarm/
#directory progetto phase1
SRC_DIR = ../phase1/support/
INCL_DIR = ../phase1/include/
#librerie linker
LIB_UARM = $(INCL_UARM)ldscripts/elf32ltsarm.h.uarmcore.x -o test.elf $(INCL_UARM)crtso.o $(INCL_UARM)libuarm.o
#object file
OBJECTS =  pcb.o asl.o test.o
#header file
HEAD_FILE = $(INCL_DIR)const.h $(INCL_DIR)clist.h $(INCL_DIR)pcb.h $(INCL_DIR)asl.h
#header file uarm
HEAD_UARM = $(INCL_UARM)uARMconst.h $(INCL_UARM)uARMtypes.h $(INCL_UARM)libuarm.h

all: test

test: $(OBJECTS)
	$(UL) $(FLAG_UL) $(LIB_UARM) $(OBJECTS)
	$(UC) $(FLAG_UC) test.elf

pcb.o: $(SRC_DIR)pcb.c $(HEAD_FILE)
	$(CC) $(FLAG_CC) pcb.o $(SRC_DIR)pcb.c

asl.o: $(SRC_DIR)asl.c $(HEAD_FILE)
	$(CC) $(FLAG_CC) asl.o $(SRC_DIR)asl.c

test.o: test.c $(HEAD_UARM) $(HEAD_FILE)
	$(CC) $(FLAG_CC) test.o test.c

clean:
	rm -rf *.o test.elf

cleanall:
	rm -rf *.o test test.elf.core.uarm test.elf.stab.uarm
