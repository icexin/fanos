#include <time.h>
extern unsigned long ticks;

long sys_ticks()
{
	return ticks;
}

