ARCH_UARM =	arm-none-eabi-
FLAG_UARM = -mcpu=arm7tdmi -c -o
INCL_UARM = /usr/include/uarm/
SRC_DIR = src/
INCL_DIR = include/
EXEC_DIR = exec/
LIB_UARM = $(INCL_UARM)ldscripts/elf32ltsarm.h.uarmcore.x -o $(EXEC_DIR)jaeOS.elf $(INCL_UARM)crtso.o $(INCL_UARM)libuarm.o
OBJECTS = p1test.o pcb.o asl.o
HEAD_FILE = include/const.h include/clist.h include/pcb.h include/asl.h 
HEAD_UARM = $(INCL_UARM)uARMconst.h $(INCL_UARM)uARMtypes.h $(INCL_UARM)libuarm.h

all: jaeOS

jaeOS: $(OBJECTS)
	mkdir -p $(EXEC_DIR)
	$(ARCH_UARM)ld -T $(LIB_UARM) $(OBJECTS)
	elf2uarm -k $(EXEC_DIR)jaeOS.elf

pcb.o: pcb.c $(HEAD_FILE)
	$(ARCH_UARM)gcc $(FLAG_UARM) pcb.o pcb.c

asl.o: asl.c $(HEAD_FILE)
	$(ARCH_UARM)gcc $(FLAG_UARM) asl.o asl.c

p1test.o: p1test.c $(HEAD_UARM) $(HEAD_FILE)
	$(ARCH_UARM)gcc $(FLAG_UARM) p1test.o p1test.c

clean:
	rm -rf *.o $(EXEC_DIR)jaeOS

cleanall:
	rm -rf *.o $(EXEC_DIR)jaeOS $(EXEC_DIR)jaeOS.core.uarm $(EXEC_DIR)jaeOS.stab.uarm $(EXEC_DIR)
