#include <process.h>
#include <mmu_paging.h>
#include <mmu_frames.h>
#include <string.h>
#include <kernel.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <mmu_heap.h>
#include <x86.h>
#include <elf.h>
#include <stdio.h>
#include <window_manager.h>

task_t *task_list_head = NULL;
task_t *task_list_last = NULL;
task_t *task_list_current = NULL;

bool task_list_is_empty() {
  return task_list_head == NULL;
}

task_t* task_list_insert(task_t* new_task) {
  new_task->next = NULL;
  new_task->prev = NULL;
  
  if(task_list_is_empty()) {
    //make it the last new_task
    task_list_last = new_task;
    task_list_head = new_task;
    task_list_current = task_list_last;
  } else {
    //make new_task a new last new_task
    task_list_last->next = new_task;     
    //mark old last node as prev of new new_task
    new_task->prev = task_list_last;
  }

  //point last to new last node
  task_list_last = new_task;

  return new_task;
}

void task_list_delete(task_t* task) {
  if(task_list_head == NULL) {
    return;
  }
  if(task == task_list_head) {
    //change first to point to next link
    task_list_head = task_list_head->next;
  } else {
    //bypass the task link
    task->prev->next = task->next;
  }    

  if(task == task_list_last) {
    //change task_list_last to point to prev link
    task_list_last = task->prev;
  } else {
    task->next->prev = task->prev;
  }
}

task_t* task_find_by_id(uint32_t id) {
  for(task_t* task = task_list_head; task != NULL; task = task->next){
    if (task->id == id) {
      return task;
    }
  }
  return NULL;
}

void task_list_dump() {
  DEBUG("Task lsit dump:\n");
  for(task_t* current = task_list_head; current != NULL; current = current->next){
    DEBUG("Task: %i\n", current->id);
  }
}

task_t* create_task_struct(bool create_pde) {
  task_t* new_task = malloc(sizeof(task_t));
  new_task->kstack = malloc(KERNEL_STACK_SIZE);
  if (create_pde) {
    new_task->pde = malloc(sizeof(pde_t) * 512);
  }
  return new_task;
}

bool elf_check_file(Elf64_Ehdr *hdr) {
  if(!hdr) return false;
  if(hdr->e_ident[EI_MAG0] != ELFMAG0) {
    DEBUG("ELF[Error]: Header EI_MAG0 incorrect.\n");
    return false;
  }
  if(hdr->e_ident[EI_MAG1] != ELFMAG1) {
    DEBUG("ELF[Error]: Header EI_MAG1 incorrect.\n");
    return false;
  }
  if(hdr->e_ident[EI_MAG2] != ELFMAG2) {
    DEBUG("ELF[Error]: Header EI_MAG2 incorrect.\n");
    return false;
  }
  if(hdr->e_ident[EI_MAG3] != ELFMAG3) {
    DEBUG("ELF[Error]: Header EI_MAG3 incorrect.\n");
    return false;
  }
  return true;
}

bool elf_check_supported(Elf64_Ehdr *hdr) {
  if(!elf_check_file(hdr)) {
    DEBUG("ELF[Error]: Invalid ELF File.\n");
    return false;
  }
  if(hdr->e_ident[EI_CLASS] != ELFCLASS64) {
    DEBUG("ELF[Error]: Unsupported ELF File Class.\n");
    return false;
  }
  if(hdr->e_ident[EI_DATA] != ELFDATA2LSB) {
    DEBUG("ELF[Error]: Unsupported ELF File byte order.\n");
    return false;
  }
  if(hdr->e_machine != EM_X86_64) {
    DEBUG("ELF[Error]: Unsupported ELF File target.\n");
    return false;
  }
  if(hdr->e_ident[EI_VERSION] != EV_CURRENT) {
    DEBUG("ELF[Error]: Unsupported ELF File version.\n");
    return false;
  }
  // if(hdr->e_type != ET_REL && hdr->e_type != ET_EXEC) {
  if(hdr->e_type != ET_EXEC) {
    DEBUG("ELF[Error]: Unsupported ELF File type.\n");
    return false;
  }
  return true;
}

uint64_t load_elf(void* elf_data_buffer, void* process_buffer) {
  Elf64_Ehdr *header = elf_data_buffer;
  assert(elf_check_supported(header));

  DEBUG("***** Preparing ELF PROCESS\n");

  DEBUG("ELF: Header e_type: %i\n",  header->e_type);
  DEBUG("ELF: Header e_machine: %i\n",  header->e_machine);
  DEBUG("ELF: Header e_version: %i\n",  header->e_version);
  DEBUG("ELF: Header e_entry: %i\n",  header->e_entry);
  DEBUG("ELF: Header e_phoff: %i\n",  header->e_phoff);
  DEBUG("ELF: Header e_shoff: %i\n",  header->e_shoff);
  DEBUG("ELF: Header e_flags: %i\n",  header->e_flags);
  DEBUG("ELF: Header e_ehsize: %i\n",  header->e_ehsize);
  DEBUG("ELF: Header e_phentsize: %i\n",  header->e_phentsize);
  DEBUG("ELF: Header e_phnum: %i\n",  header->e_phnum);
  DEBUG("ELF: Header e_shentsize: %i\n",  header->e_shentsize);
  DEBUG("ELF: Header e_shnum: %i\n",  header->e_shnum);
  DEBUG("ELF: Header e_shstrndx: %i\n",  header->e_shstrndx);
  // assert(header->e_phnum == 1); 

  Elf64_Phdr *prog_header = (Elf64_Phdr*)(((char*)elf_data_buffer) + header->e_phoff);
  DEBUG("ELF: P Header p_type;: 0x%X\n", prog_header->p_type);
  DEBUG("ELF: P Header p_flags: 0x%X\n", prog_header->p_flags);
  DEBUG("ELF: P Header p_offse: 0x%X\n", prog_header->p_offset);
  DEBUG("ELF: P Header p_vaddr: 0x%X\n", prog_header->p_vaddr);
  DEBUG("ELF: P Header p_paddr: 0x%X\n", prog_header->p_paddr);
  DEBUG("ELF: P Header p_files: 0x%X\n", prog_header->p_filesz);
  DEBUG("ELF: P Header p_memsz: 0x%X\n", prog_header->p_memsz);
  DEBUG("ELF: P Header p_align: 0x%X\n", prog_header->p_align);

  assert(prog_header->p_vaddr == 0);
  assert(prog_header->p_filesz <= PAGE_SIZE);
  // 1 = load - clear p_memsz bytes at p_vaddr to 0, then copy p_filesz bytes from p_offset to p_vaddr;
  void *source = ((char*)elf_data_buffer) + prog_header->p_offset;
  memset(process_buffer, 0, prog_header->p_memsz);
  memcpy(process_buffer, source, prog_header->p_filesz);

  for (int i=0; i<32; i++) {
    DEBUG("%X ", ((uint8_t*)process_buffer)[i]);
  }
  DEBUG("\n");

  return header->e_entry;
}

static uint32_t __task_ids = 0;

task_t* create_kernel_process(void* entry_point) {
  task_t* task = create_task_struct(true);

  uint64_t* rsp = (uint64_t*)(task->kstack + KERNEL_STACK_SIZE);
  rsp--;
  uint64_t jmp_rsp = (uint64_t)rsp;
  *rsp-- = GDT_KERNEL_DATA;       // SS
  *rsp-- = jmp_rsp;               // RSP
  *rsp-- = 0x286;                 // RFLAGS
  *rsp-- = GDT_KERNEL_CODE;       // CS
  *rsp-- = (uint64_t)entry_point; // RIP
  *rsp-- = 0;
  *rsp-- = 0;
  *rsp-- = 0;
  *rsp-- = 0;
  *rsp-- = 0;
  *rsp-- = 0;
  *rsp-- = 0;
  *rsp-- = 0;
  *rsp-- = 0;
  *rsp-- = 0;
  *rsp-- = 0;
  *rsp-- = 0;
  *rsp-- = 0;
  *rsp-- = 0;
  *rsp = 0;

  memset(task->pde, 0, sizeof(pde_t) * 512);

  task->rsp = (uint64_t)rsp; 
  task->id = __task_ids++;
  task->attribute = PROCESS_ATTR_KERNEL_SPACE;
  task->state = PROCESS_STATE_READY;

  DEBUG("PROC: Kernel task created : x%X) entry:0x%X rsp:0x%X\n", entry_point, task->rsp);
  task_list_insert(task);

  return task;
}

task_t* create_user_process(void* elf_raw_data) {
  task_t* task = create_task_struct(true);

  // Allocate memory for task (2MB max)
  uint64_t phys_addr;
  uint8_t* task_memory = alloc_frame_temp(&phys_addr);
  DEBUG("PROC: create_process_user phys_addr: 0x%X virt_addr: 0x%X\n", phys_addr, task_memory);
  uint64_t entry_point = load_elf(elf_raw_data, task_memory);
  
  uint64_t* rsp = (uint64_t*)(task->kstack + KERNEL_STACK_SIZE);
  rsp--;
  *rsp-- = GDT_USER_DATA | 3;  // SS
  *rsp-- = 0x200000 - 8;       // RSP
  *rsp-- = 0x286;              // RFLAGS
  *rsp-- = GDT_USER_CODE | 3;  // CS
  *rsp-- = entry_point;        // RIP
  *rsp-- = 0;
  *rsp-- = 0;
  *rsp-- = 0;
  *rsp-- = 0;
  *rsp-- = 0;
  *rsp-- = 0;
  *rsp-- = 0;
  *rsp-- = 0;
  *rsp-- = 0;
  *rsp-- = 0;
  *rsp-- = 0;
  *rsp-- = 0;
  *rsp-- = 0;
  *rsp-- = 0;
  *rsp = 0;

  memset(task->pde, 0, sizeof(pde_t) * 512);

  task->rsp = (uint64_t)rsp; //- switch will do it
  task->id = __task_ids++;
  task->attribute = PROCESS_ATTR_USER_SPACE;
  task->state = PROCESS_STATE_READY;
  task->pde[0].all = phys_addr |  PAGE_PRESENT_CPL3; // Present + Write + CPL3

  DEBUG("PROC: Task created : tm0: 0x%X (p:0x%X) rsp:0x%X\n", task_memory, phys_addr, task->rsp);
  task_list_insert(task);

  return task;
}

task_t *create_user_process_clone(task_t *parent, uint64_t entry_point, uint64_t user_stack) {
  task_t *task = create_task_struct(false);
  uint64_t *rsp = (uint64_t*)(task->kstack + KERNEL_STACK_SIZE);
  rsp--;
  *rsp-- = GDT_USER_DATA | 3;  // SS
  *rsp-- = user_stack;       // RSP
  *rsp-- = 0x286;              // RFLAGS
  *rsp-- = GDT_USER_CODE | 3;  // CS
  *rsp-- = entry_point;        // RIP
  *rsp-- = 0;
  *rsp-- = 0;
  *rsp-- = 0;
  *rsp-- = 0;
  *rsp-- = 0;
  *rsp-- = 0;
  *rsp-- = 0;
  *rsp-- = 0;
  *rsp-- = 0;
  *rsp-- = 0;
  *rsp-- = 0;
  *rsp-- = 0;
  *rsp-- = 0;
  *rsp-- = 0;
  *rsp = 0;

  task->rsp = (uint64_t)rsp; //- switch will do it
  task->id = __task_ids++;
  task->attribute = PROCESS_ATTR_USER_SPACE | PROCESS_ATTR_CHILD;
  task->state = PROCESS_STATE_READY;
  // have same pagging mapping as parent (same memory space)
  task->pde = parent->pde;
  task->parent = parent;

  DEBUG("PROC: Child Task created : parent id: %i, rsp:0x%X\n", parent->id, task->rsp);
  task_list_insert(task);

  return task;
}


task_t* create_user_process_file(char *filename) {
  DEBUG("PROC: Creating user process from file %s\n", filename);
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    DEBUG("PROC: File does not exist\n");
    return NULL;
  }

  uint32_t size = fsize(file);
  void *buffer = malloc(size + 512); // do we need + 512 ?
  assert(buffer != NULL);
  uint32_t read_bytes = fread(buffer, size, 1, file);
  assert(read_bytes == size);
  fclose(file);

  task_t *new_task = create_user_process(buffer);
  free(buffer);
  return new_task;
}

static void free_allocated_frames(task_t *task) {
  for (int i=0; i<512; i++) {
    if (task->pde[i].all != 0) {
      uint64_t frame_addr = task->pde[i].all ^ PAGE_PRESENT_CPL3;
      DEBUG("PROC: Free frame 0x%X\n", frame_addr);
      free_frame(frame_addr);
    }
  }
}

void kill_process_no_schedule(task_t* task) {
  if (task->id == 0) {
    HALT_AND_CATCH_FIRE("PROC: WARNING! - Cannot kill kernel idle task!");
    return;
  }
  DEBUG("PROC: KILL-PROCESS with id:%i\n", task->id);
  // Close window if any
  if (task->window != NULL) {
    window_close(task->window);
  }
  // Free Allocated frames
  free_allocated_frames(task);
  // Remove task from
  task_list_delete(task);
  free(task);
}

void kill_process(task_t* task) {

  // First kill all children (if any)
  for(task_t* child = task_list_head; child != NULL; child = child->next){
    if (child->parent == task) {
      kill_process_no_schedule(child);
    }
  }
  // Kill task
  kill_process_no_schedule(task);
  // Schedule next call
  task_list_current = task_list_head;
  do_first_task_jump();
}

bool kill_process_id(uint32_t id) {
  task_t* task = task_find_by_id(id);
  if (task != NULL) {
    kill_process(task);
    return true;
  }
  return false;
}

task_t* current_task() {
  return task_list_current;
}

task_t* next_task() {
  task_t* task = task_list_current;
  do {
    task = task->next;
    if (task == NULL) {
      task = task_list_head;
    }
  } while(task->state == PROCESS_STATE_WAIT);
  task_list_current = task;
  return task;
}

void __switch_to(task_t* next) {
  // Set TSS stack pointer for interrupt return
  tss64.rsp0 = (uint64_t)(next->kstack + KERNEL_STACK_SIZE - 8);
  // Change states. Is this best moment?
  task_list_current->state = PROCESS_STATE_READY;
  next->state = PROCESS_STATE_RUNNING;
  
  if (next->attribute & PROCESS_ATTR_USER_SPACE) {
    // Copy user tables
    memcpy(pde_user, next->pde, 512 * sizeof(pde_t));
    x86_tlb_flush_all();
  }
}

void do_first_task_jump() {
  __asm__ __volatile__ ("jmp irq0_first_jump");
}

void schedule() {
  __asm__ __volatile__ ("int $32");

}

