CFLAGS:=-nostdlib -nostdinc -fno-builtin -fno-stack-protector -g -I../include

C_SRC:=fs.c

ASM_SRC:= 

OBJ:=$(ASM_SRC:%.S=%.o) $(C_SRC:%.c=%.o) 


.PHONY:all clean


all: .depend fs.o

-include .depend

.depend:$(C_SRC)
	gcc $(CFLAGS) -M $^ > $@

fs.o:$(OBJ)
	gcc $(CFLAGS) -r  -o$@ $^

clean:
	rm -f *.o
	rm -f .depend
