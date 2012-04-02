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
