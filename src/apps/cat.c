#include <unistd.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
	char buff[30*1024];
	int ret = 0;
	char* file_name = argv[1];
	int total = open(file_name, buff);
	if (total < 0) {
		printf("no such file `%s'\n", file_name);
		return -1;
	}
	int cnt = 0;
	while (cnt < total) {
		int len = write(STDOUT_FILENO, buff + cnt, total - cnt);
		cnt += len;
	}
	return 0;
}
