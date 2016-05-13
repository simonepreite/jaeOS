# MAKEFILE WORK IN PROGRESS

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
SRC = src/
SRC_SUP = $(SRC)support/
SRC_KER = $(SRC)kernel/
INCL_DIR = include/
EXEC_DIR = exec/
#librerie linker
LIB_UARM = $(INCL_UARM)ldscripts/elf32ltsarm.h.uarmcore.x -o $(EXEC_DIR)jaeOS.elf $(INCL_UARM)crtso.o $(INCL_UARM)libuarm.o
#object file (probabilmente verranno separati in futuro)
OBJECTS = $(SRC_SUP)pcb.o $(SRC_SUP)asl.o $(SRC_KER)initial.o $(SRC_KER)exeptions.o $(SRC_KER)scheduler.o $(SRC_KER)interrupts.o $(SRC_KER)p2test.o
#header file (probabilmente verranno separati in futuro)
HEAD_FILE = $(INCL_DIR)const.h $(INCL_DIR)clist.h $(INCL_DIR)pcb.h $(INCL_DIR)asl.h $(INCL_DIR)exeptions.h $(INCL_DIR)initial.h $(INCL_DIR)interrupts.h $(INCL_DIR)scheduler.h $(INCL_DIR)types.h
#header file uarm
HEAD_UARM = $(INCL_UARM)uARMconst.h $(INCL_UARM)uARMtypes.h $(INCL_UARM)libuarm.h

all: jaeOS

jaeOS: $(OBJECTS)
	mkdir -p $(EXEC_DIR)
	$(UL) $(FLAG_UL) $(LIB_UARM) $(OBJECTS)
	$(UC) $(FLAG_UC) $(EXEC_DIR)jaeOS.elf

phase1: $(SRC_SUP)pcb.o $(SRC_SUP)asl.o $(SRC_SUP)p1test.o 
	mkdir -p phase1_exec
	$(UL) $(FLAG_UL) $(INCL_UARM)ldscripts/elf32ltsarm.h.uarmcore.x -o phase1_exec/phase1.elf $(INCL_UARM)crtso.o $(INCL_UARM)libuarm.o $(SRC_SUP)pcb.o $(SRC_SUP)asl.o $(SRC_SUP)p1test.o
	$(UC) $(FLAG_UC) phase1_exec/phase1.elf

$(SRC_SUP)p1test.o: $(SRC_SUP)p1test.c $(HEAD_UARM) $(HEAD_FILE)
	$(CC) $(FLAG_CC) $(SRC_SUP)p1test.o $(SRC_SUP)p1test.c

$(SRC_SUP)pcb.o: $(SRC_SUP)pcb.c $(HEAD_FILE)
	$(CC) $(FLAG_CC) $(SRC_SUP)pcb.o $(SRC_SUP)pcb.c

$(SRC_SUP)asl.o: $(SRC_SUP)asl.c $(HEAD_FILE)
	$(CC) $(FLAG_CC) $(SRC_SUP)asl.o $(SRC_SUP)asl.c

$(SRC_KER)initial.o: $(SRC_KER)initial.c $(HEAD_FILE)
	$(CC) $(FLAG_CC) $(SRC_KER)initial.o $(SRC_KER)initial.c

$(SRC_KER)exeptions.o: $(SRC_KER)exeptions.c $(HEAD_FILE)
	$(CC) $(FLAG_CC) $(SRC_KER)exeptions.o $(SRC_KER)exeptions.c

$(SRC_KER)scheduler.o: $(SRC_KER)scheduler.c $(HEAD_FILE)
	$(CC) $(FLAG_CC) $(SRC_KER)scheduler.o $(SRC_KER)scheduler.c

$(SRC_KER)interrupts.o: $(SRC_KER)interrupts.c $(HEAD_FILE)
	$(CC) $(FLAG_CC) $(SRC_KER)interrupts.o $(SRC_KER)interrupts.c

$(SRC_KER)p2test.o: $(SRC_KER)p2test.c $(HEAD_UARM) $(HEAD_FILE)
	$(CC) $(FLAG_CC) $(SRC_KER)p2test.o $(SRC_KER)p2test.c

clean:
	rm -rf *.o $(EXEC_DIR)jaeOS.elf

clean_p1:
	rm -rf $(SRC_SUP)*.o phase1_exec

cleanall:
	rm -rf $(SRC_KER)*.o $(SRC_SUP)*.o $(EXEC_DIR)jaeOS $(EXEC_DIR)jaeOS.core.uarm $(EXEC_DIR)jaeOS.stab.uarm $(EXEC_DIR)
