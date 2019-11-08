#include <process.h>
#include <mmu_pagging.h>
#include <string.h>
#include <kernel.h>
#include <stdlib.h>
#include <x86.h>

task_t tasks[3];
int current_task_index = 0;

void create_process_system(task_t* task, void *entry, void *stack) {
  // setup task MMU
  uint64_t* rsp = (uint64_t*)stack;
  rsp--;
  uint64_t jump_rsp = (uint64_t)rsp;
  /*
  +------------+
  +40 | %SS        |
  +32 | %RSP       |
  +24 | %RFLAGS    |
  +16 | %CS        |
  +8 | %RIP       | <-- %RSP
  +------------+
  * */
  *rsp-- = GDT_KERNEL_DATA;    // SS
  *rsp-- = jump_rsp;           // RSP
  *rsp-- = 0x286;              // RFLAGS
  *rsp-- = GDT_KERNEL_CODE;    // CS
  *rsp-- = (uint64_t)entry;    // entry point
  *rsp-- = 1;
  *rsp-- = 2;
  *rsp-- = 3;
  *rsp-- = 4;
  *rsp-- = 5;
  *rsp-- = 6;
  *rsp-- = 7;
  *rsp-- = 8;
  *rsp-- = 9;
  *rsp-- = 10;
  *rsp-- = 11;
  *rsp-- = 12;
  *rsp-- = 13;
  *rsp-- = 14;
  *rsp-- = 15;

  // task->attribute = PROCESS_ATTR_KERNEL_SPACE;
  task->id = rand();
  task->rsp = (uint64_t)(rsp+1); //- switch will do it
  task->pde.all = 0; // Not used by system process/thread
 }

void create_process_user(task_t* task, void* app_data, uint32_t app_size) {
  // setup task MMU
  uint64_t phys_addr;
  uint8_t* task_memory = alloc_frame_temp(&phys_addr);
  DEBUG("PROC: create_process_user p addr ... 0x%X\n\r", phys_addr);
  memcpy(task_memory, app_data, app_size);
  uint64_t* rsp = (uint64_t*)task_memory;
  rsp--;
  /*
  +------------+
  +40 | %SS        |
  +32 | %RSP       |
  +24 | %RFLAGS    |
  +16 | %CS        |
  +8 | %RIP       | <-- %RSP
  +------------+
  * */
  *rsp-- = GDT_USER_DATA | 3;	             // SS
  *rsp-- = 0x200000 - 8; 		   // RSP  2MB - 8
  *rsp-- = 0x286; 		         // RFLAGS
  *rsp-- = GDT_USER_CODE | 3;		           // CS
  *rsp-- = 0x0000000000000000; // entry point is always 0
  *rsp-- = 1;
  *rsp-- = 2;
  *rsp-- = 3;
  *rsp-- = 4;
  *rsp-- = 5;
  *rsp-- = 6;
  *rsp-- = 7;
  *rsp-- = 8;
  *rsp-- = 9;
  *rsp-- = 10;
  *rsp-- = 11;
  *rsp-- = 12;
  *rsp-- = 13;
  *rsp-- = 14;
  *rsp-- = 15;
  // rsp -= 14; // rax,rbx,rcx,rdx,rsi,rdi,rbp,r8,r9,r10,r11,r12,r13,r14,r15 - switch.asm irq0

  task->rsp = (uint64_t)(rsp+1); //- switch will do it
  // task->attribute = PROCESS_ATTR_USER_SPACE;
  task->id = rand();
  task->pde.all = phys_addr | 0x87; // Present + Write + CPL3
  DEBUG("PROC: Task created : tm0: 0x%X (p:0x%X) rsp:0x%X\n\r", task_memory, phys_addr, task->rsp);
 }

task_t* next_task() {
	current_task_index++;
	current_task_index = current_task_index % 3;

	return &tasks[current_task_index];
}

task_t* current_task() {
	return &tasks[current_task_index];
}

void __switch_to(task_t* next) {
  kprintf_xy(0, 0, "Task : 0x%X", next->pde);
  pde_user[0] = next->pde;
  x86_tlb_flush_all();
}

void do_first_task_jump() {
  __asm__ __volatile__ ("jmp irq0_first_jump");
}



