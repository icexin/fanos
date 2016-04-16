#ifndef FANOS_FMT_H
#define FANOS_FMT_H

#define log(fmt, ...)
#define assert(expr)
#define panic(expr)

#include <fanos/type.h>

int vsnprintf(char *str, uint32 n, const char *fmt, char* args);
int snprintf(char *str, uint32 n, const char *fmt, ...);

#endif
