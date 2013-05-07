#include <stdio.h>
#include <unistd.h>
#include <sys.h>
#include <stdarg.h>

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

int execv(char *name, char** argv)
{
	syscall2(SYS_EXEC, name, argv);
}

int execl(char* name, ...)
{
	char* argv[10] = {NULL};
	char** p = argv;
	char* argp = NULL;
	va_list ap;
	va_start(ap, name);
	do {
		argp = va_arg(ap, char*);
		*p++ = argp;
	} while(argp != NULL);
	*p = NULL;
	return execv(name, argv);
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

void sleep(size_t msec)
{
	syscall1(SYS_SLEEP, msec);
}

int open(char* name, char* buf)
{
	syscall2(SYS_OPEN, name, buf);
}
