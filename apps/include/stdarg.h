#ifndef __STDARG_H
#define __STDARG_H

typedef __builtin_va_list va_list;

#define va_start __builtin_va_start
#define va_end __builtin_va_end
#define va_arg __builtin_va_arg

#endif
