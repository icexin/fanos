#include <kernel.h>
#include <tty.h>
#include <unistd.h>


void panic(char *str)
{
	fprintf(STDERR_FILENO, "kernel panic:%s\n", str);
	for(;;);
}
