#ifndef _DEBUG_H
#define _DEBUG_H

#define debug() do{__asm__("xchg %bx,%bx");}while(0);
#endif
