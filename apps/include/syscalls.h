#ifndef __SYSCALLS_H
#define __SYSCALLS_H

#include <stdint.h>

#define SYSCall_console_putcxy 	0
#define SYSCall_console_clear 	1
#define SYSCall_windows_create 	2
#define SYSCall_memory_sbrk     3
#define SYSCall_debug_puts      4
#define SYSCall_windows_get_message	5

extern uint64_t syscall(uint64_t sys_no, ...);

#endif