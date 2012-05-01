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

char *getline(char *str, int len)
{
	char *p = str;
	do{
		read(0, p, 1);
	}while(*p++ !='\n' && --len);

	*--p = 0;
	return str;
}

