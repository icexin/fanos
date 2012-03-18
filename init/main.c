#include <kernel.h>
void move_to_user();
struct{
	long a;
	long b;
}tet={0,0};
int main(void *mbd, unsigned int magic)
{
	init_gdt();
	init_idt();
	init_timer();
	tty_clear();
	move_to_user();
	__asm__("ljmp %0"::"m"(tet));
	while(1);
	return 0;
}
