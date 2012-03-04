#ifndef _KERNEL_H
#define _KERNEL_H

#include <type.h>
#include <protect.h>
#include <global.h>
#include <const.h>

char create_descriptor(DESCRIPTOR* d, u32 base, u32 limit, u16 flag);
void init_gdt();
void init_idt_desc(unsigned char vector, u8 desc_type, int_handler handler, unsigned char privilege);
void init_idt();
void init_8259A();
void out_byte(u16 port, u8 out_data);
u8 in_byte(u16 port);

#endif
