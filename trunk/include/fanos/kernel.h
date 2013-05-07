#ifndef _KERNEL_H
#define _KERNEL_H

#include <unistd.h>
#include <fanos/time.h>
#include <fanos/system.h>

#ifndef NULL
#define NULL (void*)0
#endif

#define assert(exp) do{ \
if(!(exp)){ \
	printk("exp:%s\nfile:%s\nbase_file:%s\nline:%d\n", \
		#exp, __FILE__, __BASE_FILE__, __LINE__); \
	for(;;);\
}\
}while(0)

#ifdef DEBUG

#define log(fmt, ...) do{\
eflags_t eflag; \
disable_hwint(eflag); \
printk("[%d %s:%d]: ", sys_ticks(), __FILE__, __LINE__); \
printk(fmt, ##__VA_ARGS__); \
restore_hwint(eflag); \
}while(0)

#else

#define log(fmt, ...)

#endif

void panic(char *str);
int printk(const char *fmt, ...);
void hex_dump(char *buf, int len);

#endif
