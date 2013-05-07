#include <stdio.h>
#include <unistd.h>

int main()
{

	printf("hello! it's a app\n");
	printf("pid=%d\n", getpid());
	
	printf("spawn new process\n");
	int pid = fork();
	int status = 0;

	if (pid) { //parent
		printf("parent\n");
		wait(&status);
	} else {
		printf("son\n");
		int ticks = get_ticks();
		printf("ticks=%d\n", ticks);
		exit(ticks % 10);
	}
	return status;
}
