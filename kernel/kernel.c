#include <kernel.h>
#include <tty.h>
#include <unistd.h>


void panic(char *str)
{
	fprintf(STDERR_FILENO, "kernel panic:%s\n", str);
	for(;;);
}

int log(const char *fmt, ...)
{
	static char tmp[256];
	int n;
	char* ap = (char*)((char*)&fmt + 4);
	n = vsprintf(tmp, fmt, ap);
	write(2, tmp, n);
	return n;
}

void hex_dump(char *buf, int len)
{
	int i;
	printk("%x:", 0);
	for(i=0; i<len; i++){
		if(i % 16 == 0 && i != 0){
			printk("\n%x:", i);
		}
		printk("%x ", *(unsigned char*)buf++);
	}
	printk("\n");
}

int printk(const char *fmt, ...)
{
	static char tmp[256];
	int n;
	char* ap = (char*)((char*)&fmt + 4);
	n = vsprintf(tmp, fmt, ap);
	rs_write(tmp, n);
	return n;
}


