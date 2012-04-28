#include <unistd.h>

char *gets(char *str)
{
	char *p = str;
	do{
		read(0, p, 1);
	}while(*p++ != '\n');
	*--p = 0;
	return str;
}

