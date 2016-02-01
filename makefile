CC = arm-none-eabi-
FLAG_CC = -mcpu=arm7tdmi -c -o
INCL_UARM = /usr/include/uarm/
SRC_DIR = src/
INCL_DIR = include/
EXEC_DIR = exec/
LIB_UARM = $(INCL_UARM)ldscripts/elf32ltsarm.h.uarmcore.x -o $(EXEC_DIR)jaeOS.elf $(INCL_UARM)crtso.o $(INCL_UARM)libuarm.o
OBJECTS = $(SRC_DIR)p1test.o $(SRC_DIR)pcb.o $(SRC_DIR)asl.o
HEAD_FILE = $(INCL_DIR)const.h $(INCL_DIR)clist.h $(INCL_DIR)pcb.h $(INCL_DIR)asl.h 
HEAD_UARM = $(INCL_UARM)uARMconst.h $(INCL_UARM)uARMtypes.h $(INCL_UARM)libuarm.h

all: jaeOS

jaeOS: $(OBJECTS)
	mkdir -p $(EXEC_DIR)
	$(CC)ld -T $(LIB_UARM) $(OBJECTS)
	elf2uarm -k $(EXEC_DIR)jaeOS.elf

$(SRC_DIR)pcb.o: $(SRC_DIR)pcb.c $(HEAD_FILE)
	$(CC)gcc $(FLAG_CC) $(SRC_DIR)pcb.o $(SRC_DIR)pcb.c

$(SRC_DIR)asl.o: $(SRC_DIR)asl.c $(HEAD_FILE)
	$(CC)gcc $(FLAG_CC) $(SRC_DIR)asl.o $(SRC_DIR)asl.c

$(SRC_DIR)p1test.o: $(SRC_DIR)p1test.c $(HEAD_UARM) $(HEAD_FILE)
	$(CC)gcc $(FLAG_CC) $(SRC_DIR)p1test.o $(SRC_DIR)p1test.c

clean:
	rm -rf *.o $(EXEC_DIR)jaeOS

cleanall:
	rm -rf $(SRC_DIR)*.o $(EXEC_DIR)jaeOS $(EXEC_DIR)jaeOS.core.uarm $(EXEC_DIR)jaeOS.stab.uarm $(EXEC_DIR)
