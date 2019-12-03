#include <multiboot.h>
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
#include <mmu_paging.h>
#include <mmu_heap.h>
#include <mouse.h>
#include <assert.h>
#include <vesa.h>
#include <window_manager.h>
#include <pci.h>
#include <ata.h>
#include <fat.h>
#include <stdio.h>

#define GDT_NULL        0x00
#define GDT_KERNEL_CODE 0x08
#define GDT_KERNEL_DATA 0x10
#define GDT_USER_CODE   0x18
#define GDT_USER_DATA   0x20
#define GDT_TSS         0x28 

int serial_printf_help(unsigned c, void *ptr __UNUSED__) {
  serial_write_com(1, c);
  return 0;
}

void HALT_AND_CATCH_FIRE(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  (void)_printf(fmt, args, serial_printf_help, NULL);
  va_end(args);
  x86_hlt(); 
  while(1) {}
}

void _kdebug(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  (void)_printf(fmt, args, serial_printf_help, NULL);
  va_end(args);
}

void idle() {
  while(1) {
    x86_hlt();
  }
}

void kmain(unsigned long magic __UNUSED__, multiboot_info_t* mbi_phys) {
  init_kernel_serial();
  init_kernel_vesa(TO_VMA_PTR(multiboot_info_t *, mbi_phys));
  init_kernel_paging();

  init_kernel_pic();
  init_kernel_isr();
  init_kernel_timer();
  init_kernel_keyboard();
  init_kernel_mouse();
  init_kernel_ata();
  init_kernel_fat();

  init_kernel_window_manager();

  create_kernel_process((void*)idle);
  create_user_process_file("/apps/desktop/desktop");

  do_first_task_jump();
  while(1) { }
}
