#include <schedule.h>
#include <sys.h>

extern void sys_write();
extern void sys_read();
extern void sys_ticks();
extern void sys_fork();
extern void sys_exec();
extern void sys_exit();
extern void sys_pid();
extern void sys_waitpid();


fn_ptr sys_call_table[]={sys_write, sys_ticks, sys_fork, sys_exec, \
	sys_exit,sys_pid, sys_read, sys_waitpid};


