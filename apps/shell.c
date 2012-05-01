#include <stdio.h>
#include <unistd.h>
#include <debug.h>
#include <string.h>

int main()
{

	char buf[50];
	int status;
	printf("welcome to fanos!\n");
	while(1){
		printf("$ ");
		getline(buf, 50);
		if(!strcmp(buf, "hello")){
			int pid = fork();
			if(!pid){
				debug();
				printf("execute hello\n");
				exec("hello");
			}else{
				wait(&status);
				printf("exit:%d\n", status);
			}
		}else{
			printf("nothing happened\n");
		}
	}


	return 0;
}
