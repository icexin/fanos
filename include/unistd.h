#ifndef _UNISTD_H
#define _UNISTD_H

#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

int exec(char*name);
int fork();
long get_ticks();
int write(int fd, char *buf, int len);
int getpid();
void exit(int status);

#endif
