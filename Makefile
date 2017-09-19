# TODO: need rewrite!
CC = arm-none-eabi-gcc
LD = arm-none-eabi-ld
CFLAGS = -ansi -pedantic -Wall -Wextra -march=armv6 -O2 \
		 -msoft-float -fPIC -mapcs-frame -marm -std=c99 -I$(INC)
LDFLAGS = -N -Ttext=0x10000
BUILD = ./build
OBJ = bootstrap.o main.o context_switch.o display.o \
	  terminal.o keyboard.o shell.o string.o utils.o filesystem.o \
	  memory.o window.o
INC = ./include

kernel.elf : $(OBJ)
	$(LD) $(LDFLAGS) $(foreach o, $(OBJ), $(BUILD)/$(o)) -o $(BUILD)/kernel.elf

bootstrap.o : arch/arm/versatilepb/bootstrap.s
	$(CC) $(CFLAGS) -c arch/arm/versatilepb/bootstrap.s -o $(BUILD)/bootstrap.o

main.o : kernel/main.c $(INC)/versatilepb.h $(INC)/display.h
	$(CC) $(CFLAGS) -c kernel/main.c -o $(BUILD)/main.o

context_switch.o : arch/arm/versatilepb/context_switch.s
	$(CC) $(CFLAGS) -c arch/arm/versatilepb/context_switch.s -o $(BUILD)/context_switch.o

display.o : drivers/display.c $(INC)/display.h
	$(CC) $(CFLAGS) -c drivers/display.c -o $(BUILD)/display.o

terminal.o : drivers/terminal.c $(INC)/display.h
	$(CC) $(CFLAGS) -c drivers/terminal.c -o $(BUILD)/terminal.o

keyboard.o : drivers/keyboard.c $(INC)/display.h
	$(CC) $(CFLAGS) -c drivers/keyboard.c -o $(BUILD)/keyboard.o

shell.o : kernel/shell.c $(INC)/display.h $(INC)/shell.h
	$(CC) $(CFLAGS) -c kernel/shell.c -o $(BUILD)/shell.o

string.o : libc/string.c $(INC)/libc/string.h
	$(CC) $(CFLAGS) -c libc/string.c -o $(BUILD)/string.o

utils.o : kernel/utils.c $(INC)/utils.h $(INC)/versatilepb.h $(INC)/display.h
	$(CC) $(CFLAGS) -c kernel/utils.c -o $(BUILD)/utils.o

filesystem.o : kernel/filesystem.c $(INC)/filesystem.h $(INC)/libc/string.h
	$(CC) $(CFLAGS) -c kernel/filesystem.c -o $(BUILD)/filesystem.o

memory.o : kernel/memory.c $(INC)/memory.h
	$(CC) $(CFLAGS) -c kernel/memory.c -o $(BUILD)/memory.o

window.o : kernel/window.c $(INC)/display.h
	$(CC) $(CFLAGS) -c kernel/window.c -o $(BUILD)/window.o

.PHONY : run
run : kernel.elf
	qemu-system-arm -M versatilepb -cpu arm1176 -kernel $(BUILD)/kernel.elf -serial stdio -sd ./system.img

.PHONY : clean
clean :
	rm $(BUILD)/kernel.elf $(foreach o, $(OBJ), $(BUILD)/$(o))
