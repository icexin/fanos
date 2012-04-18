#include <unistd.h>

int write(int fd, char *buf, int len)
{
	__asm__ __volatile__("mov $0, %%eax;int $0x80"
		::"b"(fd),"c"(buf),"d"(len)
	);
}

long get_ticks()
{
	__asm__ __volatile__("mov $1, %eax; int $0x80");
}

int fork()
{
	__asm__ __volatile__ ("mov $2, %%eax;int $0x80;":::"eax");
}

void exit(int status)
{
	__asm__ __volatile__ ("mov $4, %%eax\n\t" \
			      "mov %0, %%ebx\n\t" \
			      "int $0x80;"::"b"(status):"eax");
}

int exec(char *name)
{
	__asm__ __volatile__ (
	"mov $3, %%eax\n" \
	"mov %0, %%ebx\n" \
	"int $0x80;"::"r"(name):"eax","ebx");
}

int getpid()
{
	__asm__ __volatile__("mov $5, %eax; int $0x80");
}
