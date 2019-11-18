#ifndef __KLIKA_OS_H
#define __KLIKA_OS_H

#include <syscalls.h>

#define __UNUSED__ __attribute__((unused))
#define DEBUG _debug

#define PAGE_SIZE (2*1024*1024)


void _debug(const char *fmt, ...);

#endif