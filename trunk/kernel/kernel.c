#include <kernel.h>
#include <global.h>

void keyboard();
void timer();
void taska();
void taskb();


void init_gdt()
{
	u16 *gdt_ptr_limit = (u16*)gdt_ptr; //gdt界限
	u32 **gdt_ptr_base  = (u32**)(gdt_ptr + 2);//gdt基址

	init_gdt_desc(0, 0, 0, 0); //空段
	init_gdt_desc(1, 0, 0xFFFFF, DA_CR | DA_32 | DA_LIMIT_4K); //代码段
	init_gdt_desc(2, 0, 0xFFFFF, DA_DRW | DA_32 | DA_LIMIT_4K); //数据段
	init_gdt_desc(3, (u32)(ldt + 0), sizeof(DESCRIPTOR)* 4, DA_LDT);
	init_gdt_desc(4, (u32)(ldt + 1), sizeof(DESCRIPTOR)* 4, DA_LDT);
	init_gdt_desc(5, (u32)(tss + 0), sizeof(TSS), DA_386TSS);
	init_gdt_desc(6, (u32)(tss + 1), sizeof(TSS), DA_386TSS);

	init_ldt();
	init_tss();

	*gdt_ptr_limit = GDT_SIZE;
	*gdt_ptr_base = (u32*)gdt;
	__asm__ __volatile__("lgdt gdt_ptr\n\t");

}

void init_ldt()
{
	init_ldt_desc(0, 0, 0xFFFFF);
	init_ldt_desc(1, 0, 0xFFFFF);
}

void init_tss()
{
	int i;
	for(i=0; i<2; i++)
	{
		tss[i].esp0 = 0x100000 + 0x4000;
		tss[i].ss0 = 0x10;
		tss[i].eflags = 0x200;
		tss[i].esp = 0x100000 + 0x100000;
		tss[i].es = 0x17;
		tss[i].cs = 0x0F;
		tss[i].ss = 0x17;
		tss[i].ds = 0x17;
		tss[i].fs = 0x17;
		tss[i].gs = 0x17;
	}

	tss[0].esp0 = 0x100000 + 0x100000;
	tss[1].esp0 = 0x100000 + 0x200000;
	tss[0].eip = taska;
	tss[1].eip = taskb;
	tss[0].ldt = 0x18;
	tss[1].ldt = 0x20;
}

 
void create_descriptor(DESCRIPTOR *d, u32 base, u32 limit, u16 flag)
{
	d->limit_low = limit & 0xFFFF; 
	d->base_low = base & 0xFFFF;
	d->base_mid = (base >> 16) & 0xFF;
	d->attr1 = flag & 0xFF;
	d->limit_high_attr2 = ((limit >> 16) & 0x0F) | ((flag >> 8) & 0xF0);
	d->base_high = (base >> 24) & 0xFF;
}

void init_gdt_desc(unsigned char vector, u32 base, u32 limit, u16 flag)
{
	create_descriptor(gdt + vector, base, limit, flag);
}	

void init_ldt_desc(unsigned char vector, u32 base, u32 limit)
{
	DESCRIPTOR *d = ldt[vector];
	create_descriptor(d, 0, 0, 0);
	create_descriptor(d + 1, base, limit, DA_DPL3 | DA_32 | DA_CR | DA_LIMIT_4K );
	create_descriptor(d + 2, base, limit, DA_DPL3 | DA_32 | DA_DRW | DA_LIMIT_4K );
}



void init_idt_desc(unsigned char vector, u8 desc_type,
			  int_handler handler, unsigned char privilege)
{
	GATE *	p_gate	= &idt[vector];
	u32	base	= (u32)handler;
	p_gate->offset_low	= base & 0xFFFF;
	p_gate->selector	= SELECTOR_KERNEL_CS;
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


void init_8259A()
{
	/* Master 8259, ICW1. */
	out_byte(INT_M_CTL,	0x11);

	/* Slave  8259, ICW1. */
	out_byte(INT_S_CTL,	0x11);

	/* Master 8259, ICW2. 设置 '主8259' 的中断入口地址为 0x20. */
	out_byte(INT_M_CTLMASK,	INT_VECTOR_IRQ0);

	/* Slave  8259, ICW2. 设置 '从8259' 的中断入口地址为 0x28 */
	out_byte(INT_S_CTLMASK,	INT_VECTOR_IRQ8);

	/* Master 8259, ICW3. IR2 对应 '从8259'. */
	out_byte(INT_M_CTLMASK,	0x4);

	/* Slave  8259, ICW3. 对应 '主8259' 的 IR2. */
	out_byte(INT_S_CTLMASK,	0x2);

	/* Master 8259, ICW4. */
	out_byte(INT_M_CTLMASK,	0x1);

	/* Slave  8259, ICW4. */
	out_byte(INT_S_CTLMASK,	0x1);

}

void init_idt()
{
	u16 *idt_ptr_limit = (u16*)idt_ptr; //idt界限
	u32 **idt_ptr_base  = (u32**)(idt_ptr + 2);//idt基址

	init_8259A();
	init_idt_desc(0x21, DA_386IGate, keyboard, PRIVILEGE_KRNL);
	init_idt_desc(0x20, DA_386IGate, timer, PRIVILEGE_KRNL);

	//8259a主芯片的中断mask
	out_byte(INT_M_CTLMASK,	0xFE);

	//8259a从芯片的中断mask
	out_byte(INT_S_CTLMASK,	0xFF);

	*idt_ptr_limit = IDT_SIZE * sizeof(GATE) - 1;
	*idt_ptr_base = (u32*)idt;
	__asm__ __volatile__("lidt idt_ptr\n\t");

}

#define CLK_8253 1193180
void init_timer()
{
	out_byte(0x43, 0x36); //控制字：通道0工作在方式3，计数初值采用二进制
	out_byte(0x40, CLK_8253 / 1000 & 0xFF ); //100hz
	out_byte(0x40, (CLK_8253 / 1000 >> 8 ) & 0xFF);
}
