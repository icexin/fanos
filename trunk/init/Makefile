CFLAGS:=-nostdlib -nostdinc -fno-builtin -fno-stack-protector -g -I../include

C_SRC:=main.c

ASM_SRC:= 

OBJ:=$(ASM_SRC:%.S=%.o) $(C_SRC:%.c=%.o) 


.PHONY:all clean


all: .depend init.o

-include .depend

.depend:$(C_SRC)
	gcc $(CFLAGS) -M $^ > $@

init.o:$(OBJ)
	gcc $(CFLAGS) -r  -o$@ $^

clean:
	rm -f *.o
	rm -f .depend
