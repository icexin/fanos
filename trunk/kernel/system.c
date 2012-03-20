#include <system.h>
#include <schedule.h>




unsigned char gdt_ptr[6];
struct desc gdt[GDT_SIZE];

u8		idt_ptr[6];	/* 0~15:Limit  16~47:Base */
struct gate		idt[IDT_SIZE]; /*idt表*/



void init_gdt()
{
	u16 *gdt_ptr_limit = (u16*)gdt_ptr; //gdt界限
	u32 **gdt_ptr_base  = (u32**)(gdt_ptr + 2);//gdt基址

	create_gdt_desc(gdt, 0, 0, 0); //空段
	create_gdt_desc(gdt + 1, 0, 0xFFFFF, DA_CR | DA_32 | DA_LIMIT_4K); //代码段
	create_gdt_desc(gdt + 2, 0, 0xFFFFF, DA_DRW | DA_32 | DA_LIMIT_4K); //数据段
	

	*gdt_ptr_limit = GDT_SIZE;
	*gdt_ptr_base = (u32*)gdt;
	__asm__ __volatile__("lgdt gdt_ptr\n\t");

}


 
void create_gdt_desc(struct desc *d, u32 base, u32 limit, u16 flag)
{
	d->limit_low = limit & 0xFFFF; 
	d->base_low = base & 0xFFFF;
	d->base_mid = (base >> 16) & 0xFF;
	d->attr1 = flag & 0xFF;
	d->limit_high_attr2 = ((limit >> 16) & 0x0F) | ((flag >> 8) & 0xF0);
	d->base_high = (base >> 24) & 0xFF;
}

void create_ldt_desc(struct desc *d, u32 base, u32 limit, u16 flag)
{
	create_gdt_desc(d, base, limit, flag);
}


void create_idt_desc(struct gate *p_gate, u8 desc_type,
			  int_handler handler, unsigned char privilege)
{
	u32	base	= (u32)handler;
	p_gate->offset_low	= base & 0xFFFF;
	p_gate->selector	= 0x08;
	p_gate->dcount		= 0;
	p_gate->attr		= desc_type | (privilege << 5);
	p_gate->offset_high	= (base >> 16) & 0xFFFF;
}



u8 in_byte(u16 port)
{
	__asm__(
	"mov %0, %%dx\n\t"
	"xor %%eax, %%eax \n\t"
	"in %%dx, %%al\n\t"
	"nop\n\t"
	"nop\n\t"
	"nop\n\t"
	"nop\n\t"
	"nop\n\t"
	"nop\n\t"::"r"(port):"eax","edx");
}

void out_byte(u16 port, u8 out_data)
{
	__asm__(
	"mov %0, %%dx\n\t"
	"out %1, %%dx\n\t"
	"nop\n\t"
	"nop\n\t"
	"nop\n\t"
	"nop\n\t"
	"nop\n\t"::"r"(port),"a"(out_data):"edx");
}



