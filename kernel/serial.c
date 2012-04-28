#include <system.h>

#define PORT 0x3f8
void rs_init()
{
	out_byte(0x3FB, 0x80); /* set DLAB */
	out_byte(0x3F8, 0x30); /* low divisor */
	out_byte(0x3F9, 0x00); /* heigh divisor */
	out_byte(0x3FB, 0x03); /* 8 data bits, 1 stop bit */
	out_byte(0x3FC, 0x03); /* set query mode */
	out_byte(PORT + 1, 0x00); /* disable all intrs */
}

int rs_write(char *buf, int len)
{
	
	int i;
	cli();
	for(i=0; i<len; i++){
		out_byte(0x3F8, buf[i]);
		while( !(0x40 & in_byte(0x3FD)));
	}
	sti();
	return i;
}
