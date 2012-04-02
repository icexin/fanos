#ifndef _SYS_H
#define _SYS_H
#include <schedule.h>

extern void sys_write();
extern void sys_ticks();


fn_ptr sys_call_table[]={sys_write, sys_ticks};
#endif
