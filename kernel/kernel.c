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
	var_list ap = (var_list)((char*)&fmt + 4);
	n = vsprintf(tmp, fmt, ap);
	write(2, tmp, n);
	return n;
}

void hex_dump(char *buf, int len)
{
	int i;
	printf("%x:", 0);
	for(i=0; i<len; i++){
		if(i % 16 == 0 && i != 0){
			putchar('\n');
			printf("%x:", i);
		}
		printf("%x ", *(unsigned char*)buf++);
	}
	putchar('\n');
}
