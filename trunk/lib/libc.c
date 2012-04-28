#include <unistd.h>
#include <sys.h>
#include <debug.h>

int write(int fd, char *buf, int len)
{
	syscall3(SYS_WRITE, fd, buf, len);
}

int read(int fd, char *buf, int len)
{
	syscall3(SYS_READ, fd, buf, len);
}

long get_ticks()
{
	syscall0(SYS_TICKS);
}

int fork()
{
	syscall0(SYS_FORK);
}

void exit(int status)
{
	syscall1(SYS_EXIT, status);
}

int exec(char *name)
{
	syscall1(SYS_EXEC, name);
}

int getpid()
{
	syscall0(SYS_GETPID);
}
int waitpid(int pid, int *status)
{
	syscall2(SYS_WAITPID, pid, status);
}

int wait(int *status)
{
	return waitpid(-1, status);
}
