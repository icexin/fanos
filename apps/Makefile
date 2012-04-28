CFLAGS:=-nostdlib -nostdinc -fno-builtin -fno-stack-protector -I../include
LIBS:=../lib/lib.o


.PHONY:all clean


all: hello shell
	cp  $^ ../ramfs/
	sync

hello:head.o hello.o ../lib/lib.o
	ld -o$@ $^ -Ttext=0x1000

shell:head.o shell.o ../lib/lib.o
	ld -o$@ $^ -Ttext=0x1000
clean:
	rm -f *.o
	rm -f .depend
