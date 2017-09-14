# TODO: need rewrite!
CC = arm-none-eabi-gcc
LD = arm-none-eabi-ld
CFLAGS = -ansi -pedantic -Wall -Wextra -march=armv6 -O2 \
		 -msoft-float -fPIC -mapcs-frame -marm -std=c99 -I$(INC)
LDFLAGS = -N -Ttext=0x10000
BUILD = ./build
OBJ = bootstrap.o kernel.o context_switch.o syscalls.o display.o terminal.o keyboard.o
INC = ./include

kernel.elf : $(OBJ)
	$(LD) $(LDFLAGS) $(foreach o, $(OBJ), $(BUILD)/$(o)) -o $(BUILD)/kernel.elf

bootstrap.o : arch/arm/versatilepb/bootstrap.s
	$(CC) $(CFLAGS) -c arch/arm/versatilepb/bootstrap.s -o $(BUILD)/bootstrap.o

kernel.o : kernel/kernel.c $(INC)/versatilepb.h $(INC)/asm.h $(INC)/display.h
	$(CC) $(CFLAGS) -c kernel/kernel.c -o $(BUILD)/kernel.o

context_switch.o : arch/arm/versatilepb/context_switch.s
	$(CC) $(CFLAGS) -c arch/arm/versatilepb/context_switch.s -o $(BUILD)/context_switch.o

syscalls.o : arch/arm/versatilepb/syscalls.s
	$(CC) $(CFALGS) -c arch/arm/versatilepb/syscalls.s -o $(BUILD)/syscalls.o

display.o : drivers/display.c $(INC)/display.h
	$(CC) $(CFLAGS) -c drivers/display.c -o $(BUILD)/display.o

terminal.o : drivers/terminal.c $(INC)/display.h
	$(CC) $(CFLAGS) -c drivers/terminal.c -o $(BUILD)/terminal.o

keyboard.o : drivers/keyboard.c $(INC)/display.h
	$(CC) $(CFLAGS) -c drivers/keyboard.c -o $(BUILD)/keyboard.o

.PHONY : run
run : kernel.elf
	qemu-system-arm -M versatilepb -cpu arm1176 -kernel $(BUILD)/kernel.elf -serial stdio

.PHONY : clean
clean :
	rm $(BUILD)/kernel.elf $(foreach o, $(OBJ), $(BUILD)/$(o))
