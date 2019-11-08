#ifndef __PRINTF_H
#define __PRINTF_H

typedef int (*fnptr_t)(unsigned c, void *helper);

int _printf(const char *fmt, va_list args, fnptr_t fn, void *ptr);

#endif