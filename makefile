LIB_UARM = /usr/include/uarm/ldscripts/elf32ltsarm.h.uarmcore.x -o jaeOS /usr/include/uarm/crtso.o /usr/include/uarm/libuarm.o
OBJECTS = p1test.o pcb.o asl.o
HEAD_FILE = include/const.h include/clist.h include/pcb.h include/asl.h 
HEAD_UARM = /usr/include/uarm/uARMconst.h /usr/include/uarm/uARMtypes.h /usr/include/uarm/libuarm.h

all: jaeOS

jaeOS: $(OBJECTS)
	arm-none-eabi-ld -T $(LIB_UARM) $(OBJECTS)
	elf2uarm -k jaeOS

pcb.o: pcb.c $(HEAD_FILE)
	arm-none-eabi-gcc -mcpu=arm7tdmi -c -o pcb.o pcb.c

asl.o: asl.c $(HEAD_FILE)
	arm-none-eabi-gcc -mcpu=arm7tdmi -c -o asl.o asl.c

p1test.o: p1test.c $(HEAD_UARM) $(HEAD_FILE)
	arm-none-eabi-gcc -mcpu=arm7tdmi -c -o p1test.o p1test.c

clean:
	rm -rf *o jaeOS

cleanall:
	rm -rf *o jaeOS jaeOS.core.uarm jaeOS.stab.uarm *.o
