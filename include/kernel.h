#ifndef _KERNEL_H
#define _KERNEL_H

#include <unistd.h>


#define assert(exp) do{ \
if(!(exp)){ \
	fprintf(STDOUT_FILENO, "exp:%s\nfile:%s\nbase_file:%s\nline:%d\n", \
		#exp, __FILE__, __BASE_FILE__, __LINE__); \
	for(;;);\
}\
}while(0)

void panic(char *str);
int log(const char *fmt, ...);

#endif
