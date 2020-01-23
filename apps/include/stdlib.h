#ifndef __STDLIB_H
#define __STDLIB_H

#include <stdarg.h>

typedef int (*fnptr_t)(unsigned c, void *helper);

int rand();
int rand_range(int lower, int upper);
void srand(unsigned int seed);

int _printf(const char *fmt, va_list args, fnptr_t fn, void *ptr);
void sprintf(char* buf, const char* fmt, ...);

int abs(int n);
long int labs(long int n);
long long int llabs(long long int n);

#endif