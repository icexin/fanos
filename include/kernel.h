#ifndef _KERNEL_H
#define _KERNEL_H

#include <type.h>
#include <protect.h>
#include <const.h>

void init_gdt_desc(unsigned char vector, u32 base, u32 limit, u16 flag);
void init_gdt();
void init_idt_desc(unsigned char vector, u8 desc_type, int_handler handler, unsigned char privilege);
void init_idt();
void init_8259A();
void out_byte(u16 port, u8 out_data);
u8 in_byte(u16 port);
void init_timer();
void init_ldt_desc(unsigned char vector, u32 base, u32 limit);
void init_ldt();
void init_tss();
void create_descriptor(DESCRIPTOR *d, u32 base, u32 limit, u16 flag);


typedef struct _tss
{
	u32 backlink;
	u32 esp0;
	u32 ss0;
	u32 esp1;
	u32 ss1;
	u32 esp2;
	u32 ss2;
	u32 cr3;
	u32 eip;
	u32 eflags;
	u32 eax;
	u32 ecx;
	u32 edx;
	u32 ebx;
	u32 esp;
	u32 ebp;
	u32 esi;
	u32 edi;
	u32 es;
	u32 cs;
	u32 ss;
	u32 ds;
	u32 fs;
	u32 gs;
	u32 ldt;
	u16 trap;
	u16 iobase;
	char stack[1024 * 4];
}TSS;


#endif
