#ifndef _TIME_H
#define _TIME_H
#include <stdint.h>

long sys_ticks();
void sys_sleep(size_t msec);

#endif
