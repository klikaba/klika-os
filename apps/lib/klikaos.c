#include <klikaos.h>
#include <syscalls.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

int __debug_helper_index;
#define MAX_DEBUG_BUFFER_SIZE 256

static int debug_printf_helper(unsigned c, void *ptr) {
	if (__debug_helper_index < MAX_DEBUG_BUFFER_SIZE) {
 		((char*)ptr)[__debug_helper_index++] = c;
	}
  return 0;
}

void _debug(const char* fmt, ...) {
	char buf[MAX_DEBUG_BUFFER_SIZE];
  memset(buf, 0, MAX_DEBUG_BUFFER_SIZE);
  __debug_helper_index = 0;
  va_list args;
  va_start(args, fmt);
  (void)_printf(fmt, args, debug_printf_helper, buf);
  va_end(args);
  syscall(SYSCall_debug_puts, buf);
}

void memory_stats(mmu_frame_stats_t *stats_out) {
  syscall(SYSCall_memory_stats, stats_out);
}