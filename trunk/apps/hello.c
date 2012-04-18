#include <stdio.h>
#include <unistd.h>

int main()
{

	printf("hello! it's a app\n");
	int pid = fork();
	printf("pid=%d\n", getpid());
	if(pid){
		int i;
		for(i=0; i<1000; i++){
			printf("father:%d\n", i);
		}
		exit(0);
	}else{
		int i;
		for(i=1; i<5000; i++){
			printf("son:%d\n", i);
		}
		exit(0);
	}
	return 0;
}
