#compilatore
CC = arm-none-eabi-gcc
#flag compilatore
FLAG_CC = -mcpu=arm7tdmi -c -o
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
SRC_DIR = src/
INCL_DIR = include/
EXEC_DIR = exec/
#librerie linker
LIB_UARM = $(INCL_UARM)ldscripts/elf32ltsarm.h.uarmcore.x -o $(EXEC_DIR)jaeOS.elf $(INCL_UARM)crtso.o $(INCL_UARM)libuarm.o
#object file
OBJECTS = $(SRC_DIR)p1test.o $(SRC_DIR)pcb.o $(SRC_DIR)asl.o
#header file
HEAD_FILE = $(INCL_DIR)const.h $(INCL_DIR)clist.h $(INCL_DIR)pcb.h $(INCL_DIR)asl.h 
#header file uarm
HEAD_UARM = $(INCL_UARM)uARMconst.h $(INCL_UARM)uARMtypes.h $(INCL_UARM)libuarm.h

all: jaeOS

jaeOS: $(OBJECTS)
	mkdir -p $(EXEC_DIR)
	$(UL) $(FLAG_UL) $(LIB_UARM) $(OBJECTS)
	$(UC) $(FLAG_UC) $(EXEC_DIR)jaeOS.elf

$(SRC_DIR)pcb.o: $(SRC_DIR)pcb.c $(HEAD_FILE)
	$(CC) $(FLAG_CC) $(SRC_DIR)pcb.o $(SRC_DIR)pcb.c

$(SRC_DIR)asl.o: $(SRC_DIR)asl.c $(HEAD_FILE)
	$(CC) $(FLAG_CC) $(SRC_DIR)asl.o $(SRC_DIR)asl.c

$(SRC_DIR)p1test.o: $(SRC_DIR)p1test.c $(HEAD_UARM) $(HEAD_FILE)
	$(CC) $(FLAG_CC) $(SRC_DIR)p1test.o $(SRC_DIR)p1test.c

clean:
	rm -rf *.o $(EXEC_DIR)jaeOS.elf

cleanall:
	rm -rf $(SRC_DIR)*.o $(EXEC_DIR)jaeOS $(EXEC_DIR)jaeOS.core.uarm $(EXEC_DIR)jaeOS.stab.uarm $(EXEC_DIR)
