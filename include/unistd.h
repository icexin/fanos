#ifndef _UNISTD_H
#define _UNISTD_H

#include <stdint.h>

#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

int execv(char*name, char* argv[]);
int execl(char* name, ...);
int fork();
long get_ticks();
int write(int fd, char *buf, int len);
int read(int fd, char *buf, int len);
int getpid();
void exit(int status);
int waitpid(int pid, int *status);
int wait(int *status);
void sleep(size_t msec);
int open(char* name, char* buf);

#endif
