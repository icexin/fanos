#include <time.h>
extern unsigned long ticks;

long sys_ticks()
{
	return ticks;
}

long get_ticks()
{
	__asm__ __volatile__("mov $1, %eax; int $0x80");
}
