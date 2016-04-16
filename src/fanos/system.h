#ifndef _SYSTEM_H
#define _SYSTEM_H

#include <fanos/type.h>

#define GDT_SIZE 128
#define IDT_SIZE 256

/* 描述符类型值说明 */
#define	DA_32			0x4000	/* 32 位段*/
#define	DA_LIMIT_4K		0x8000	/* 段界限粒度为 4K 字节*/
#define	DA_DPL0			0x00	/* DPL = 0*/
#define	DA_DPL1			0x20	/* DPL = 1*/
#define	DA_DPL2			0x40	/* DPL = 2*/
#define	DA_DPL3			0x60	/* DPL = 3*/
/* 存储段描述符类型值说明 */
#define	DA_DR			0x90	/* 存在的只读数据段类型值*/
#define	DA_DRW			0x92	/* 存在的可读写数据段属性值*/
#define	DA_DRWA			0x93	/* 存在的已访问可读写数据段类型值*/
#define	DA_C			0x98	/* 存在的只执行代码段属性值*/
#define	DA_CR			0x9A	/* 存在的可执行可读代码段属性值*/
#define	DA_CCO			0x9C	/* 存在的只执行一致代码段属性值	*/
#define	DA_CCOR			0x9E	/* 存在的可执行可读一致代码段属性值*/
/* 系统段描述符类型值说明 */
#define	DA_LDT			0x82	/* 局部描述符表段类型值*/
#define	DA_TaskGate		0x85	/* 任务门类型值*/
#define	DA_386TSS		0x89	/* 可用 386 任务状态段类型值*/
#define	DA_386CGate		0x8C	/* 386 调用门类型值*/
#define	DA_386IGate		0x8E	/* 386 中断门类型值*/
#define	DA_386TGate		0x8F	/* 386 陷阱门类型值*/

#define	PRIVILEGE_KRNL	0
#define	PRIVILEGE_TASK	1
#define	PRIVILEGE_USER	3

/* 中断向量号 */
#define IRQ_TIMER 0
#define IRQ_KEYBOARD 1
#define IRQ_SLAVE 2

/* timer relate */
#define CLK_8253 1193180
#define TIMER_HZ 100

#define cli() __asm__("cli")
#define sti() __asm__("sti")

/* 门描述符 */
struct gate
{
	uint16	offset_low;	/* Offset Low */
	uint16	selector;	/* Selector */
	uint8	dcount;		/* 该字段只在调用门描述符中有效。如果在利用
				   调用门调用子程序时引起特权级的转换和堆栈
				   的改变，需要将外层堆栈中的参数复制到内层
				   堆栈。该双字计数字段就是用于说明这种情况
				   发生时，要复制的双字参数的数量。*/
	uint8	attr;		/* P(1) DPL(2) DT(1) TYPE(4) */
	uint16	offset_high;	/* Offset High */
}__attribute__((packed));

/* 存储段描述符/系统段描述符 */
struct desc		/* 共 8 个字节 */
{
	uint16	limit_low;		/* Limit */
	uint16	base_low;		/* Base */
	uint8	base_mid;		/* Base */
	uint8	attr1;			/* P(1) DPL(2) DT(1) TYPE(4) */
	uint8	limit_high_attr2;	/* G(1) D(1) 0(1) AVL(1) LimitHigh(4) */
	uint8	base_high;		/* Base */
}__attribute__((packed));

extern struct desc gdt[];

void init_gdt();
void create_gdt_desc(struct desc *d, uint32 base, uint32 limit, uint16 flag);
void create_ldt_desc(struct desc *d, uint32 base, uint32 limit, uint16 flag);

// 只有小于或等于privilege的进程才能产生这个中断描述符描述的中断
void create_idt_desc(struct gate *p_gate, uint8 desc_type, void(*handler)(), unsigned char privilege);
void init_idt();
void init_pic();

void pic_enable(uint16 line);
void pic_disable(uint16 line);

char get_fs_byte(void* addr);
void get_fs_str(char* desc, void* src);
void get_fs_buff(char* desc, void* src, int n);
void put_fs_buff(char* desc, void* src, int n);
void get_fs_int(int* dest, int *src);
void put_fs_int(int* dest, int val);

void out_byte(uint16 port, uint8 out_data);
uint8 in_byte(uint16 port);

void out_word(uint16 port, uint16 data);
uint16 in_word(uint16 port);

void udelay(unsigned long n);
void mdelay(unsigned long n);

// the true sleep function. cpu will sleep until next interupt comes
void hlt();

typedef unsigned long eflags_t;
#define disable_hwint(eflags) __asm__("pushf; pop %0; cli;":"=g"(eflags))
#define restore_hwint(eflags) __asm__("push %0; popf;"::"g"(eflags))

#endif
