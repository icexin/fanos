#include <fanos/type.h>
#include <fanos/system.h>
#include <fanos/tty.h>
#include <fanos/fmt.h>

/* 8259A interrupt controller ports. */
#define INT_M_CTL     0x20 /* I/O port for interrupt controller       <Master> */
#define INT_M_CTLMASK 0x21 /* setting bits in this port disables ints <Master> */
#define INT_S_CTL     0xA0 /* I/O port for second interrupt controller<Slave>  */
#define INT_S_CTLMASK 0xA1 /* setting bits in this port disables ints <Slave>  */

extern struct gate idt[];
extern uint8 idt_ptr[];
extern void keyboard();
extern void timer();
extern void sys_call();

static void init_timer();
static void init_8259A();

void divide_error();
void debug();
void nmi();
void break_point();
void over_flow();
void bounds();
void invalid_op();
void coprocessor();
void double_fault();
void co_overflow();
void invalid_tss();
void invalid_seg();
void stack_error();
void general_protect();
void page_error();
void co_error();

void reverse();

void init_idt()
{
	uint16 *idt_ptr_limit = (uint16*)idt_ptr; //idt界限
	uint32 **idt_ptr_base  = (uint32**)(idt_ptr + 2);//idt基址

	int i;
	for(i=0x20; i<0xFF; i++){
		create_idt_desc(idt+i, DA_386IGate, reverse, PRIVILEGE_KRNL);
	}

	/*
	create_idt_desc(idt + 0x00, DA_386IGate, divide_error, PRIVILEGE_KRNL);
	create_idt_desc(idt + 0x01, DA_386IGate, debug, PRIVILEGE_KRNL);
	create_idt_desc(idt + 0x02, DA_386IGate, nmi, PRIVILEGE_KRNL);
	create_idt_desc(idt + 0x03, DA_386IGate, break_point, PRIVILEGE_KRNL);
	create_idt_desc(idt + 0x04, DA_386IGate, over_flow, PRIVILEGE_KRNL);
	create_idt_desc(idt + 0x05, DA_386IGate, bounds, PRIVILEGE_KRNL);
	create_idt_desc(idt + 0x06, DA_386IGate, invalid_op, PRIVILEGE_KRNL);
	create_idt_desc(idt + 0x07, DA_386IGate, coprocessor, PRIVILEGE_KRNL);
	create_idt_desc(idt + 0x08, DA_386IGate, double_fault, PRIVILEGE_KRNL);
	create_idt_desc(idt + 0x09, DA_386IGate, co_overflow, PRIVILEGE_KRNL);
	create_idt_desc(idt + 0x0A, DA_386IGate, invalid_tss, PRIVILEGE_KRNL);
	create_idt_desc(idt + 0x0B, DA_386IGate, invalid_seg, PRIVILEGE_KRNL);
	create_idt_desc(idt + 0x0C, DA_386IGate, stack_error, PRIVILEGE_KRNL);
	create_idt_desc(idt + 0x0D, DA_386IGate, general_protect, PRIVILEGE_KRNL);
	create_idt_desc(idt + 0x0E, DA_386IGate, page_error, PRIVILEGE_KRNL);
	create_idt_desc(idt + 0x10, DA_386IGate, co_error, PRIVILEGE_KRNL);
	*/

	create_idt_desc(idt + 0x20, DA_386IGate, timer, PRIVILEGE_KRNL);

	/*键盘中断*/
	create_idt_desc(idt + 0x21, DA_386IGate, keyboard, PRIVILEGE_KRNL);

	/*系统调用陷阱*/
	create_idt_desc(idt + 0x80, DA_386TGate, sys_call, PRIVILEGE_USER);



	*idt_ptr_limit = IDT_SIZE * sizeof(struct gate);
	*idt_ptr_base = (uint32*)idt;
	__asm__ __volatile__("lidt idt_ptr\n\t");

}

void init_pic()
{
	init_idt();
	init_8259A();
	//8259a主芯片的中断mask
	out_byte(INT_M_CTLMASK,	0xFF);
	//8259a从芯片的中断mask
	out_byte(INT_S_CTLMASK,	0xFF);
	/*初始化时钟中断*/
	init_timer();
	pic_enable(IRQ_TIMER);
}

static void init_8259A()
{
	/* Master 8259, ICW1. */
	out_byte(INT_M_CTL,	0x11);

	/* Slave  8259, ICW1. */
	out_byte(INT_S_CTL,	0x11);

	/* Master 8259, ICW2. 设置 '主8259' 的中断入口地址为 0x20. */
	out_byte(INT_M_CTLMASK,	0x20);

	/* Slave  8259, ICW2. 设置 '从8259' 的中断入口地址为 0x28 */
	out_byte(INT_S_CTLMASK,	0x28);

	/* Master 8259, ICW3. IR2 对应 '从8259'. */
	out_byte(INT_M_CTLMASK,	0x4);

	/* Slave  8259, ICW3. 对应 '主8259' 的 IR2. */
	out_byte(INT_S_CTLMASK,	0x2);

	/* Master 8259, ICW4. */
	out_byte(INT_M_CTLMASK,	0x1);

	/* Slave  8259, ICW4. */
	out_byte(INT_S_CTLMASK,	0x1);
}

void pic_disable(uint16 line)
{
	uint16 port = INT_M_CTLMASK;
	if (line > 8) {
		port = INT_S_CTLMASK;
		line -= 8;
	}
	out_byte(port, in_byte(port) | 1 << line);
}

void pic_enable(uint16 line)
{
	uint16 port = INT_M_CTLMASK;
	if (line > 8) {
		port = INT_S_CTLMASK;
		line -= 8;
	}
	out_byte(port, in_byte(port) & ~(1 << line));
}

static void init_timer()
{
	out_byte(0x43, 0x36); //控制字：通道0工作在方式3，计数初值采用二进制
	out_byte(0x40, CLK_8253 / TIMER_HZ & 0xFF ); //100hz
	out_byte(0x40, (CLK_8253 / TIMER_HZ >> 8 ) & 0xFF);
}

void do_reverse()
{
//	printk("Not yet handled error\n");
}

void exception_handler(int vector, int err_code, int eip, int cs, int eflags)
{
	//__asm__ __volatile__("xchg %bx,%bx");
	char * err_msg[] = {"#DE Divide Error",
			    "#DB RESERVED",
			    "—  NMI Interrupt",
			    "#BP Breakpoint",
			    "#OF Overflow",
			    "#BR BOUND Range Exceeded",
			    "#UD Invalid Opcode (Undefined Opcode)",
			    "#NM Device Not Available (No Math Coprocessor)",
			    "#DF Double Fault",
			    "    Coprocessor Segment Overrun (reserved)",
			    "#TS Invalid TSS",
			    "#NP Segment Not Present",
			    "#SS Stack-Segment Fault",
			    "#GP General Protection",
			    "#PF Page Fault",
			    "—  (Intel reserved. Do not use.)",
			    "#MF x87 FPU Floating-Point Error (Math Fault)",
			    "#AC Alignment Check",
			    "#MC Machine Check",
			    "#XF SIMD Floating-Point Exception"
	};
	log("ERROR:%s\n", err_msg[vector]);
	log("CS:%X\n", cs);
	log("EIP:%X\n", eip);
	log("EFLAGS:%X\n", eflags);
	if(err_code != 0xFFFFFFFF){
		log("ERROR CODE:%X\n", err_code);
	}
	while(1);
}
