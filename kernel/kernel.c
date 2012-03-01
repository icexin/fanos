#include <type.h>
#include <protect.h>
#include <global.h>
#include <const.h>
#include <string.h>

void clear();

char create_descriptor(DESCRIPTOR* d, u32 base, u32 limit, u16 flag);
void init_gdt();
void init_idt_desc(unsigned char vector, u8 desc_type, int_handler handler, unsigned char privilege);
void init_idt();
void init_8259A();
void out_byte(u16 port, u8 out_data);
u8 in_byte(u16 port);
void handle();
void keyboard();

void kmain( void* mbd, unsigned int magic )
{
   if ( magic != 0x2BADB002 )
   {
      /* Something went not according to specs. Print an error */
      /* message and halt, but do *not* rely on the multiboot */
      /* data structure. */
   }
   unsigned int i;
   init_tty();
   clear();
   puts("kernel start");
   init_gdt();
   init_idt();
   /* Write your kernel here. */
}



void clear()
{

   unsigned char *videoram = (unsigned char *) 0xb8000;
   int i;
   for (i=0;i<80*25;i++) {
       videoram[i*2] = ' '; /* character 'A' */
       videoram[1] = 0x07; /* forground, background color. */
   }
}

void init_gdt()
{
	u16 *gdt_ptr_limit = (u16*)gdt_ptr; //gdt界限
	u32 **gdt_ptr_base  = (u32**)(gdt_ptr + 2);//gdt基址

	create_descriptor(gdt, 0, 0, 0); //空段
	create_descriptor(gdt + 1, 0, 0xFFFFF, DA_CR | DA_32 | DA_LIMIT_4K); //代码段
	create_descriptor(gdt + 2, 0, 0xFFFFF, DA_DRW | DA_32 | DA_LIMIT_4K); //数据段

	*gdt_ptr_limit = GDT_SIZE;
	*gdt_ptr_base = (u32*)gdt;
	__asm__ __volatile__("lgdt gdt_ptr\n\t");

}
 
char create_descriptor(DESCRIPTOR *d, u32 base, u32 limit, u16 flag)
{

	d->limit_low = limit & 0xFFFF; 
	d->base_low = base & 0xFFFF;
	d->base_mid = (base >> 16) & 0xFF;
	d->attr1 = flag & 0xFF;
	d->limit_high_attr2 = ((limit >> 16) & 0x0F) | ((flag >> 8) & 0xF0);
	d->base_high = (base >> 24) & 0xFF;


	return '0' ;
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

	/* Master 8259, OCW1.  */
	out_byte(INT_M_CTLMASK,	0xFD);

	/* Slave  8259, OCW1.  */
	out_byte(INT_S_CTLMASK,	0xFF);
}

void init_idt()
{
	u16 *idt_ptr_limit = (u16*)idt_ptr; //idt界限
	u32 **idt_ptr_base  = (u32**)(idt_ptr + 2);//idt基址

	init_8259A();
	init_idt_desc(0x21, DA_386IGate, keyboard, PRIVILEGE_KRNL);


	*idt_ptr_limit = IDT_SIZE * sizeof(GATE) - 1;
	*idt_ptr_base = (u32*)idt;
	__asm__ __volatile__("lidt idt_ptr\n\t");

}


void handle()
{
	putchar('a');
	in_byte(0x60);
}

