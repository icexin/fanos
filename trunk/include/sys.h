#ifndef _SYS_H
#define _SYS_H
#include <schedule.h>

extern void sys_write();
extern void sys_ticks();
extern void sys_fork();
extern void sys_exec();
extern void sys_exit();
extern void sys_pid();


fn_ptr sys_call_table[]={sys_write, sys_ticks, sys_fork, sys_exec, sys_exit,sys_pid};
#endif
