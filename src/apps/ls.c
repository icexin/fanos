#include <unistd.h>
#include <stdio.h>
#include <fanos/fs.h>

char fsbuf[5*1024];

int main()
{
	struct dirent* ent = (struct dirent*)fsbuf;
	int len = 0;

	if ((len = open("/", &fsbuf)) < 0) {
		return -1;
	}
	int i = 0;
	for (i=0; i<len/sizeof(struct dirent); i++) {
		printf("%s\n", ent[i].name);
	}
}
