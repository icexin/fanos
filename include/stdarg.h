#ifndef _STDARG_H
#define _STDARG_H

#define _INTSIZEOF(n) ((sizeof(n)+3) & ~3)

#define va_start(ap, v) (ap=(va_list)&v + _INTSIZEOF(ap))
#define va_arg(ap, t) (*(t*)((ap+=_INTSIZEOF(t))-_INTSIZEOF(t)))
#define va_end(ap)

typedef char* va_list;

#endif
