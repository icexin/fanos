#include <type.h>
#include <system.h>
#include <sys.h>

/* 8259A interrupt controller ports. */
#define INT_M_CTL     0x20 /* I/O port for interrupt controller       <Master> */
#define INT_M_CTLMASK 0x21 /* setting bits in this port disables ints <Master> */
#define INT_S_CTL     0xA0 /* I/O port for second interrupt controller<Slave>  */
#define INT_S_CTLMASK 0xA1 /* setting bits in this port disables ints <Slave>  */



extern struct gate idt[];
extern u8   idt_ptr[];
extern void keyboard();
extern void timer();
extern void sys_call();

static void init_timer();
static void init_8259A();

void init_idt()
{
	u16 *idt_ptr_limit = (u16*)idt_ptr; //idt界限
	u32 **idt_ptr_base  = (u32**)(idt_ptr + 2);//idt基址

	
	create_idt_desc(idt + 0x20, DA_386IGate, timer, PRIVILEGE_KRNL);

	/*键盘中断*/
	create_idt_desc(idt + 0x21, DA_386IGate, keyboard, PRIVILEGE_KRNL);

	/*系统调用陷阱*/
	create_idt_desc(idt + 0x80, DA_386TGate, sys_call, PRIVILEGE_USER);

	

	*idt_ptr_limit = IDT_SIZE * sizeof(struct gate);
	*idt_ptr_base = (u32*)idt;
	__asm__ __volatile__("lidt idt_ptr\n\t");
	
	
	init_8259A();
	//8259a主芯片的中断mask
	out_byte(INT_M_CTLMASK,	0xFC);
	//8259a从芯片的中断mask
	out_byte(INT_S_CTLMASK,	0xFF);

	/*初始化时钟中断*/
	init_timer();

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


#define CLK_8253 1193180
static void init_timer()
{
	out_byte(0x43, 0x36); //控制字：通道0工作在方式3，计数初值采用二进制
	out_byte(0x40, CLK_8253 / 100 & 0xFF ); //100hz
	out_byte(0x40, (CLK_8253 / 100 >> 8 ) & 0xFF);
}
