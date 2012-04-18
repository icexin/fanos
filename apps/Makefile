CFLAGS:=-nostdlib -nostdinc -fno-builtin -fno-stack-protector -g -I../include

C_SRC:=hello.c

ASM_SRC:= head.S

OBJ:=$(ASM_SRC:%.S=%.o) $(C_SRC:%.c=%.o) 


.PHONY:all clean


all: .depend hello
	cp  hello ../ramfs/
	sync

-include .depend

.depend:$(C_SRC)
	gcc $(CFLAGS) -M $^ > $@


hello:$(OBJ) ../lib/lib.o
	ld -o$@ $^ -Ttext=0x1000

clean:
	rm -f *.o
	rm -f .depend
