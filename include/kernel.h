#ifndef _KERNEL_H
#define _KERNEL_H

#include <type.h>
#include <system.h>
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





#endif
