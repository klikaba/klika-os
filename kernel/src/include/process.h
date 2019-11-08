#ifndef __PROCESS_H
#define __PROCESS_H

#include <stdint.h>
#include <mmu_pagging.h>

#define PROCESS_ATTR_USER_SPACE		0x0000
#define PROCESS_ATTR_KERNEL_SPACE	0x0001

#define TASK_KERNEL_STACK_SIZE 512

typedef struct {
  uint64_t rsp;
  uint32_t id;
  uint32_t attribute;
  // For now we are supporting only 2MB programs :) 
  // We keep track of one entry in PDE that will be mapped to 0x0000000 (user program space)
  pde_t pde; 
}  __attribute__((packed)) task_t;

extern task_t tasks[3];
extern int current_task_index;

void create_process_system(task_t* task, void *entry, void *stack);
void create_process_user(task_t* task, void* app_data, uint32_t app_size);
task_t* next_task();
task_t* current_task();
void __switch_to(task_t* next);
void do_first_task_jump();
#endif