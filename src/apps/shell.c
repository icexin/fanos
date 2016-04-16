#include <stdio.h>
#include <unistd.h>
#include <string.h>

char** parse_arg(char* cmd, char** app)
{
	static char* arg[10] = {NULL};
	char* p = cmd;
	int i = 0;

	p = strtok(cmd, " ");
	if (p == NULL) {
		*app = NULL;
		return NULL;
	}
	*app = p;
	arg[i++] = *app;
	while ((p = strtok(NULL, " ")) != NULL) {
		arg[i++] = p;
	}
	arg[i] = NULL;
	return arg;
}

int main()
{
	char cmd[50] = "/";
	char** argv = NULL;
	char* app_name = NULL;
	printf("welcome to fanos!\n");
	while (1) {
		printf("fan@fanos:~ ");
		getline(cmd+1, 50);
		if (cmd[1] == 0) {
			continue;
		}
		argv = parse_arg(cmd, &app_name);
		int pid = fork();
		if (!pid) {
			if (execv(app_name, argv) != 0) {
				printf("no such file or command\n");
				exit(-1);
			}
		} else {
			wait(NULL);
		}
	}
	return 0;
}
