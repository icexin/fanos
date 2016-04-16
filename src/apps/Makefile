CFLAGS:=-nostdlib -nostdinc -fno-builtin -fno-stack-protector -I../include \
	-DDEBUG
LIBS:=../lib/lib.o
apps=hello shell cat test_arg ls

.PHONY:all clean


all: $(apps) *.c
	cp  $^ ../ramfs/
	sync

hello:head.o hello.o ../lib/lib.o
	ld -o$@ $^ -Ttext=0x1000

shell:head.o shell.o ../lib/lib.o
	ld -o$@ $^ -Ttext=0x1000

cat:head.o cat.o ../lib/lib.o
	ld -o$@ $^ -Ttext=0x1000

test_arg:head.o test_arg.o ../lib/lib.o
	ld -o$@ $^ -Ttext=0x1000

ls:head.o ls.o ../lib/lib.o
	ld -o$@ $^ -Ttext=0x1000

clean:
	rm -f *.o
	rm -f .depend
	rm -f $(apps)
