CFLAGS:=-nostdlib -nostdinc -fno-builtin -fno-stack-protector -g -I../include

C_SRC:= keyboard.c schedule.c trap.c hd.c kernel.c mem.c serial.c \
	system.c tty.c  time.c hd.c fork.c syscall.c  keymap.c\
	wait.c

ASM_SRC:=head.S asm.S sys_call.S 

OBJ:=$(ASM_SRC:%.S=%.o) $(C_SRC:%.c=%.o) 

.depend:$(C_SRC)
	gcc $(CFLAGS) -M $^ > $@

.PHONY:all clean
all: .depend kernel_all.o

include .depend
kernel_all.o: $(OBJ)
	gcc $(CFLAGS) -r  -o$@ $^

clean:
	rm -f *.o
	rm -f .depend
