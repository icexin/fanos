#include <kernel.h>


void panic(char *str)
{
	printf("kernel panic:%s\n", str);
	for(;;);
}
