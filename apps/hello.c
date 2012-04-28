#include <stdio.h>
#include <unistd.h>

int main()
{

	printf("hello! it's a app\n");
	printf("pid=%d\n", getpid());
	int pid = fork();

	if(pid){ //parent
		printf("parent\n");
		wait(0);
	}else{
		printf("son\n");
		int ticks = get_ticks();
		printf("ticks=%d\n", ticks);
		exit(ticks % 10);
	}

	return 0;
}
