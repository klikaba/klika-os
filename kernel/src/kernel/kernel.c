#include <multiboot.h>
#include <console.h>
#include <stdint.h>
#include <string.h>
#include <kernel.h>
#include <memory.h>
#include <elf.h>
#include <x86.h>
#include <stdarg.h>
#include <_printf.h>
#include <isr.h>
#include <timer.h>
#include <keyboard.h>
#include <timer.h>
#include <process.h>
#include <serial.h>
#include <pic.h>
#include <mmu_pagging.h>
#include <mmu_heap.h>
#include <mouse.h>
#include <assert.h>
#include <vesa.h>
#include <window_manager.h>

#define __UNUSED__ __attribute__((unused))

#define GDT_NULL        0x00
#define GDT_KERNEL_CODE 0x08
#define GDT_KERNEL_DATA 0x10
#define GDT_USER_CODE   0x18
#define GDT_USER_DATA   0x20
#define GDT_TSS         0x28 

console_t __krnl_console;

int serial_printf_help(unsigned c, void *ptr __UNUSED__) {
  serial_write_com(1, c);
  return 0;
}

void kputs(char *string) {
  console_puts(&__krnl_console, string);
}

void kprintf(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  (void)_printf(fmt, args, cprintf_help, &__krnl_console);
  va_end(args);
}

void kprintf_xy(int x, int y, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  int old_index = __krnl_console.current_index;
  __krnl_console.current_index = x*2 + y*25;
  (void)_printf(fmt, args, cprintf_help, &__krnl_console);
  __krnl_console.current_index = old_index;
  va_end(args);
}

void HALT_AND_CATCH_FIRE(const char *fmt, ...) {
  __krnl_console.color = LIGHTRED;
  va_list args;
  va_start(args, fmt);
  (void)_printf(fmt, args, cprintf_help, &__krnl_console);
  va_end(args);
  x86_hlt(); 
}

void _kdebug(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  (void)_printf(fmt, args, serial_printf_help, NULL);
  va_end(args);
}

void _kpanic(const char *fmt, ...) {
  __krnl_console.color = LIGHTRED;
  va_list args;
  va_start(args, fmt);
  (void)_printf(fmt, args, serial_printf_help, NULL);
  va_end(args);
  x86_hlt(); 
}

void init_kernel_console() {
  __krnl_console.buffer = (char*)0xFFFF8000000B8000;
  console_init(&__krnl_console);
}

#include <../../../apps/simple_win/build/simple_win_byte.c>

void kmain(unsigned long magic __UNUSED__, multiboot_info_t* mbi_phys) {
  init_kernel_serial();
  init_kernel_console();
  init_kernel_vesa(TO_VMA_PTR(multiboot_info_t *, mbi_phys));
  kprintf("\nKLIKA OS v0.1\n");
  init_kernel_pagging();
  init_kernel_window_manager();

  init_kernel_pic();
  init_kernel_isr();
  init_kernel_timer();
  init_kernel_keyboard();
  init_kernel_mouse();

  create_user_process(simple_win);
  create_user_process(simple_win);

  do_first_task_jump();
  while(1) { }
}
