
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            global.h
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/* EXTERN is defined as extern except in global.c */
#ifdef	GLOBAL_VARIABLES_HERE
#undef	EXTERN
#define	EXTERN
#else 
#define EXTERN extern
#endif

#define GDT_SIZE 128
#define IDT_SIZE 256
#define TASK_SIZE 2
#include <type.h>
#include <const.h>
#include <protect.h>
#include <kernel.h>


EXTERN	u8		gdt_ptr[6];	/* 0~15:Limit  16~47:Base */
EXTERN	DESCRIPTOR	gdt[GDT_SIZE];
EXTERN	u8		idt_ptr[6];	/* 0~15:Limit  16~47:Base */
EXTERN	GATE		idt[IDT_SIZE];
EXTERN  DESCRIPTOR  ldt[TASK_SIZE][3];
EXTERN  u8	    ldt_ptr[TASK_SIZE][6];
EXTERN  TSS     tss[TASK_SIZE];
