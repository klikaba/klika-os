#ifndef __PROCESS_H
#define __PROCESS_H

#include <stdint.h>
#include <mmu_pagging.h>

#define PROCESS_ATTR_USER_SPACE		0x0000
#define PROCESS_ATTR_KERNEL_SPACE	0x0001

#define KERNEL_STASK_SIZE 2048

typedef struct {
    uint32_t reserved0;
    uint64_t rsp0;
    uint64_t rsp1;
    uint64_t rsp2;
    uint64_t reserved1;
    uint64_t ist1; 
    uint64_t ist2; 
    uint64_t ist3; 
    uint64_t ist4; 
    uint64_t ist5; 
    uint64_t ist6; 
    uint64_t ist7; 
    uint64_t reserved2;
    uint16_t reserved3;
    uint16_t iomap_base;
} __attribute__((packed)) tss_t;

extern tss_t tss64; // from: loader.asm

typedef struct {
  uint64_t rsp;
  uint32_t id;
  uint32_t attribute;
  uint8_t kstack[KERNEL_STASK_SIZE];
  // For now we are supporting only 2MB programs :) 
  // We keep track of one entry in PDE that will be mapped to 0x0000000 (user program space)
  pde_t pde; 
}  __attribute__((packed)) task_t;

extern task_t tasks[3];
extern int current_task_index;

void create_user_process(task_t* task, void* elf_raw_data);
task_t* next_task();
task_t* current_task();
void __switch_to(task_t* next);
void do_first_task_jump();
#endif