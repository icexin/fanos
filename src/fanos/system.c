#include <fanos/type.h>
#include <fanos/system.h>

uint8 gdt_ptr[6];
struct desc gdt[GDT_SIZE];

uint8 idt_ptr[6];	/* 0~15:Limit  16~47:Base */
struct gate	idt[IDT_SIZE]; /*idt表*/

void init_gdt()
{
	// gdt_ptr分两部分,4字节的起始地址和2字节的长度.
	uint16 *gdt_ptr_limit = (uint16*)gdt_ptr; //gdt界限
	uint32 **gdt_ptr_base = (uint32**)(gdt_ptr + 2);//gdt基址

	create_gdt_desc(gdt, 0, 0, 0); //空段
	create_gdt_desc(gdt+1, 0, 0xFFFFF, DA_CR|DA_32|DA_LIMIT_4K); //代码段
	create_gdt_desc(gdt+2, 0, 0xFFFFF, DA_DRW|DA_32|DA_LIMIT_4K); //数据段
	
	*gdt_ptr_limit = GDT_SIZE * sizeof(struct desc);
	*gdt_ptr_base = (uint32*)gdt;

	// 装载gdt并置cs为代码段选择子 
	// ljmp cs, offset
	__asm__ __volatile__("lgdt gdt_ptr\n\t"
						 "mov $0x10, %%eax\n\t"
						 "mov %%ax, %%ds\n\t"
						 "mov %%ax, %%es\n\t"
						 "mov %%ax, %%fs\n\t"
						 "ljmp $0x08,$1f\n\t1:\n\t"
						 :::"eax"
	);
}
 
void create_gdt_desc(struct desc *d, uint32 base, 
	uint32 limit, uint16 flag)
{
	d->limit_low = limit & 0xFFFF; 
	d->base_low = base & 0xFFFF;
	d->base_mid = (base >> 16) & 0xFF;
	d->attr1 = flag & 0xFF;
	d->limit_high_attr2 = ((limit >> 16) & 0x0F) | ((flag >> 8) & 0xF0);
	d->base_high = (base >> 24) & 0xFF;
}

// ldt结构和gdt一样,只是选择子上的不同造就作用不同
void create_ldt_desc(struct desc *d, uint32 base,
	uint32 limit, uint16 flag)
{
	create_gdt_desc(d, base, limit, flag);
}


void create_idt_desc(struct gate *p_gate, uint8 desc_type, 
			  void(*handler)(), uint8 privilege)
{
	uint32 base = (uint32)handler;
	p_gate->offset_low	= base & 0xFFFF;
	p_gate->selector	= 0x08;
	p_gate->dcount		= 0;
	p_gate->attr		= desc_type | (privilege << 5);
	p_gate->offset_high	= (base >> 16) & 0xFFFF;
}

// get_fs_xx 从用户空间获取资源
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
	"2:movb $0, (%%ebx)\n\t" \
	::"b"(desc),"d"(src):"eax");
}

void put_fs_buff(char *desc, void *src, int n)
{
	__asm__ __volatile__(
	"1:mov (%%ebx), %%al\n\t" \
	"movb %%al, %%fs:(%%edx)\n\t" \
	"inc %%edx\n\t" \
	"inc %%ebx\n\t" \
	"loop 1b \n\t"
	::"b"(src),"d"(desc),"c"(n):"eax");
}

void get_fs_buff(char *desc, void *src, int n)
{
	__asm__ __volatile__(
	"1:mov %%fs:(%%ebx), %%al\n\t" \
	"movb %%al, (%%edx)\n\t" \
	"inc %%edx\n\t" \
	"inc %%ebx\n\t" \
	"loop 1b \n\t"
	::"b"(src),"d"(desc),"c"(n):"eax");
}

void put_fs_int(int *dest, int val)
{
	__asm__ __volatile__(
	"mov %%eax, %%fs:(%%ebx)\n\t"::"a"(val),"b"(dest));
}

void get_fs_int(int *dest, int* src)
{
	__asm__ __volatile__(
	"mov %%fs:(%%ebx), %%eax\n\t"
	"mov %%eax, (%%edx)\n\t"::"b"(src),"d"(dest):"eax");
}

uint8 in_byte(uint16 port)
{
	__asm__ __volatile__(
	"xor %%eax, %%eax \n\t"
	"in %%dx, %%al\n\t"
	"nop\n\t"
	"nop\n\t"
	"nop\n\t"
	"nop\n\t"::"d"(port):"eax");
}

void out_byte(uint16 port, uint8 out_data)
{
	__asm__ __volatile__(
	"out %%al, %%dx\n\t"
	"nop\n\t"
	"nop\n\t"
	"nop\n\t"::"d"(port),"a"(out_data));
}

uint16 in_word(uint16 port)
{
	__asm__ __volatile__(
	"xor %%eax, %%eax\n\t"
	"inw %%dx, %%ax\n\t"
	"nop\n\t"
	"nop\n\t"
	"nop\n\t"::"d"(port));
}



void out_word(uint16 port, uint16 data)
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

void hlt()
{
	__asm__("hlt\n\t");
}

