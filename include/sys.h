#ifndef _SYS_H
#define _SYS_H
#include <schedule.h>

extern sys_write(int fd, char *buf, int len);


fn_ptr sys_call_table[]={sys_write};
#endif
