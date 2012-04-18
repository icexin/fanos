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
	__asm__ __volatile__("lgdt gdt_ptr\n\t"
						 "mov $0x10, %%eax\n\t"
						 "mov %%ax, %%ds\n\t"
						 "mov %%ax, %%es\n\t"
						 "mov %%ax, %%fs\n\t"
						 "ljmp $0x08,$1f\n\t1:\n\t"
						 :::"eax"
	);

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

char get_fs_byte(void *addr)
{
	__asm__ __volatile__(
	"xor %%eax, %%eax\n\t" \
	"movb %%fs:(%0), %%al;\n\t"::"r"(addr):"eax");
}

void get_fs_str(char *desc, void *src)
{
	__asm__ __volatile__(
	"movb %%fs:(%%edx), %%al\n\t" \
	"1:test %%al, %%al\n\t" \
	"je 2f\n\t" \
	"mov %%al, (%%ebx)\n\t" \
	"incl %%ebx\n\t" \
	"incl %%edx\n\t" \
	"movb %%fs:(%%edx), %%al\n\t" \
	"jmp 1b\n\t" \
	"2:nop\n\t" \
	::"b"(desc),"d"(src):"eax");
}

u8 in_byte(u16 port)
{
	__asm__ __volatile__(
	"xor %%eax, %%eax \n\t"
	"in %%dx, %%al\n\t"
	"nop\n\t"
	"nop\n\t"
	"nop\n\t"
	"nop\n\t"::"d"(port):"eax");
}

void out_byte(u16 port, u8 out_data)
{
	__asm__ __volatile__(
	"out %%al, %%dx\n\t"
	"nop\n\t"
	"nop\n\t"
	"nop\n\t"::"d"(port),"a"(out_data));
}

u16 in_word(u16 port)
{
	__asm__ __volatile__(
	"xor %%eax, %%eax\n\t"
	"inw %%dx, %%ax\n\t"
	"nop\n\t"
	"nop\n\t"
	"nop\n\t"::"d"(port));
}



void out_word(u16 port, u16 data)
{
	__asm__ __volatile__(
	"outw %%ax, %%dx\n\t"
	"nop\n\t"
	"nop\n\t"
	"nop\n\t"::"a"(data),"d"(port));
}

void udelay(unsigned long n)
{
	if(!n)
		return;
	__asm__("1:dec %%eax; jnz 1b"::"a"(n));
}


void mdelay(unsigned long n)
{
	while(n--)
		udelay(1000);
}


