#include <fanos/kernel.h>
#include <fanos/tty.h>
#include <unistd.h>
#include <stdio.h>
#include <stdarg.h>

void panic(char *str)
{
	printk("kernel panic:%s\n", str);
	for(;;);
}

void hex_dump(char *buf, int len)
{
	int i;
	printk("%x:", (int)buf);
	for(i=0; i<len; i++){
		if(i % 16 == 0 && i != 0){
			printk("\n%x:", (int)(buf+i));
		}
		printk("%x ", *(unsigned char*)buf++);
	}
	printk("\n");
}

int printk(const char *fmt, ...)
{
	static char tmp[256];
	int n;
	va_list ap;
	va_start(ap, fmt);
	n = vsnprintf(tmp, 256, fmt, ap);
	va_end(ap);

	rs_write(tmp, n);
	return n;
}
