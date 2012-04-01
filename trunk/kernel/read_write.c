#include <unistd.h>

int write(int fd, char *buf, int len)
{
	__asm__ __volatile__("mov $0, %%eax;int $0x80"
		::"b"(fd),"c"(buf),"d"(len)
	);
}
