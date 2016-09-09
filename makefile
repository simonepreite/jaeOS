#
#	Makefile
#	Automatically compile and build Phase0/1 by using appropriate make command
#
#	Gruppo 28:
#	Del Vecchio Matteo
#	Preite Simone
#

#compilatore
CC = arm-none-eabi-gcc
#flag compilatore
FLAG_CC = -mcpu=arm7tdmi -I $(INCL_UARM) -I $(INCL_P1)
FLAG_CC_P1 = $(FLAG_CC) -c -o
FLAG_CC_P2 = $(FLAG_CC) -I $(INCL_P2) -c -o
#linker
UL = arm-none-eabi-ld
#flag linker
FLAG_UL = -T
#converitore per uARM
UC = elf2uarm
#flag converter uARM
FLAG_UC = -k
#percorso librerie uARM
INCL_UARM = /usr/include/uarm
#directory progetto
P0 = source/phase0
P1 = source/phase1
P2 = source/phase2
SRC_SUP = $(P1)/support
SRC_KER = $(P2)/kernel
INCL_P1 = $(P1)/include
INCL_P2 = $(P2)/include
COMPILED = compiled
EXEC_P1 = compiled/exec_phase1
EXEC_P2 = compiled/kernel
BUILD_P1 = $(P1)/build
BUILD_P2 = $(P2)/build
#librerie linker
LIB_UARM = $(INCL_UARM)/ldscripts/elf32ltsarm.h.uarmcore.x $(INCL_UARM)/crtso.o $(INCL_UARM)/libuarm.o
LIB_UARM_P1 = $(LIB_UARM) -o $(EXEC_P1)/phase1.elf
LIB_UARM_P2 = $(LIB_UARM) -o $(EXEC_P2)/jaeOS.elf
#object file (probabilmente verranno separati in futuro)
OBJECTS_PCB = $(BUILD_P1)/pcb.o $(BUILD_P1)/asl.o
OBJECTS_SUP = $(OBJECTS_PCB) $(BUILD_P1)/p1test.o
OBJECTS_KER = $(OBJECTS_PCB) $(BUILD_P2)/initial.o $(BUILD_P2)/exceptions.o $(BUILD_P2)/syscall.o $(BUILD_P2)/scheduler.o $(BUILD_P2)/interrupts.o $(BUILD_P2)/p2test.o
#header file (probabilmente verranno separati in futuro)
HEAD_SUP = $(INCL_P1)/const.h $(INCL_P1)/types.h $(INCL_P1)/clist.h $(INCL_P1)/pcb.h $(INCL_P1)/asl.h
HEAD_KER = $(INCL_P2)/exceptions.h $(INCL_P2)/initial.h $(INCL_P2)/interrupts.h $(INCL_P2)/scheduler.h $(INCL_P2)/syscall.h $(INCL_P1)/types.h
#header file uarm
HEAD_UARM = $(INCL_UARM)/uARMconst.h $(INCL_UARM)/uARMtypes.h $(INCL_UARM)/libuarm.h

all: jaeOS

jaeOS: compiledDir phase0 phase1 phase2

phase0: $(P0)/p0test

phase1: clean_p1 buildDir1 $(OBJECTS_SUP)	# clean_p1 da togliere dopo aver finito la phase2 perchè verrà usato clean_all nel target jaeos
	mkdir -p $(EXEC_P1)
	$(UL) $(FLAG_UL) $(LIB_UARM_P1) $(OBJECTS_SUP)
	$(UC) $(FLAG_UC) $(EXEC_P1)/phase1.elf

phase2: clean_p2 buildDir2 $(OBJECTS_KER)
	mkdir -p $(EXEC_P2)
	$(UL) $(FLAG_UL) $(LIB_UARM_P2) $(OBJECTS_KER)
	$(UC) $(FLAG_UC) $(EXEC_P2)/jaeOS.elf

compiledDir:
	mkdir -p $(COMPILED)

buildDir1:
	mkdir -p $(BUILD_P1)

buildDir2:
	mkdir -p $(BUILD_P2)

$(P0)/p0test: $(P0)/p0test.c $(P0)/clist.h
	gcc $(P0)/p0test.c -o $(P0)/p0test

$(BUILD_P1)/pcb.o: $(SRC_SUP)/pcb.c $(HEAD_SUP)
	$(CC) $(FLAG_CC_P1) $@ $(SRC_SUP)/pcb.c

$(BUILD_P1)/asl.o: $(SRC_SUP)/asl.c $(HEAD_SUP)
	$(CC) $(FLAG_CC_P1) $@ $(SRC_SUP)/asl.c

$(BUILD_P1)/p1test.o: $(SRC_SUP)/p1test.c $(HEAD_UARM) $(HEAD_SUP)
	$(CC) $(FLAG_CC_P1) $@ $(SRC_SUP)/p1test.c

$(BUILD_P2)/initial.o: $(SRC_KER)/initial.c $(HEAD_KER)
	$(CC) $(FLAG_CC_P2) $@ $(SRC_KER)/initial.c

$(BUILD_P2)/syscall.o: $(SRC_KER)/syscall.c $(HEAD_KER)
	$(CC) $(FLAG_CC_P2) $@ $(SRC_KER)/syscall.c

$(BUILD_P2)/exceptions.o: $(SRC_KER)/exceptions.c $(HEAD_KER)
	$(CC) $(FLAG_CC_P2) $@ $(SRC_KER)/exceptions.c

$(BUILD_P2)/scheduler.o: $(SRC_KER)/scheduler.c $(HEAD_KER)
	$(CC) $(FLAG_CC_P2) $@ $(SRC_KER)/scheduler.c

$(BUILD_P2)/interrupts.o: $(SRC_KER)/interrupts.c $(HEAD_KER)
	$(CC) $(FLAG_CC_P2) $@ $(SRC_KER)/interrupts.c

$(BUILD_P2)/p2test.o: $(SRC_KER)/p2test.c $(HEAD_UARM) $(HEAD_KER)
	$(CC) $(FLAG_CC_P2) $@ $(SRC_KER)/p2test.c

clean_p0:
	rm -rf $(P0)/p0test

clean_p1:
	rm -rf $(BUILD_P1) $(EXEC_P1)

clean_p2:
	rm -rf $(BUILD_P2) $(EXEC_P2)

cleanall: clean_p0
	rm -rf $(COMPILED) $(BUILD_P1) $(BUILD_P2)

