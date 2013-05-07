#include <fanos/system.h>

#define PORT 0x3f8
void rs_init()
{
	out_byte(PORT + 3, 0x80); /* set DLAB */
	out_byte(PORT + 0, 0x30); /* low divisor */
	out_byte(PORT + 1, 0x00); /* heigh divisor */
	out_byte(PORT + 3, 0x03); /* 8 data bits, 1 stop bit */
	out_byte(PORT + 4, 0x03); /* set query mode */
	out_byte(PORT + 1, 0x00); /* disable all intrs */
}

int is_transmit_empty()
{
	/* bit 5 send register is empty */
	return in_byte(PORT + 5) & 0x20;
}

int is_read_ready()
{
	/* bit 1 recv register ready */
	return in_byte(PORT + 5) & 1;
}

void rs_putchar(char ch)
{
	eflags_t eflags;
	disable_hwint(eflags);
	while (!is_transmit_empty());
	out_byte(PORT, ch);
	restore_hwint(eflags);
}

char rs_getchar() 
{
	while (!is_read_ready());
	return in_byte(PORT);
}

int rs_write(char* buff, int len)
{
	int i = 0;
	for (i=0; i<len; i++) {
		rs_putchar(buff[i]);
	}
	return i;
}

