#ifndef _SYS_H
#define _SYS_H
#include <schedule.h>


#define SYS_WRITE 0
#define SYS_TICKS 1
#define SYS_FORK 2
#define SYS_EXEC 3
#define SYS_EXIT 4
#define SYS_GETPID 5
#define SYS_READ 6
#define SYS_WAITPID 7

#define syscall0(n)       do{__asm__("int $0x80"::"a"(n));}while(0) 
#define syscall1(n,b)     do{__asm__("int $0x80"::"a"(n),"b"(b));}while(0) 
#define syscall2(n,b,c)   do{__asm__("int $0x80"::"a"(n),"b"(b),"c"(c));}while(0) 
#define syscall3(n,b,c,d) do{__asm__("int $0x80"::"a"(n),"b"(b),"c"(c),"d"(d));}while(0) 


#endif
