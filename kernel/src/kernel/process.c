#include <process.h>
#include <mmu_pagging.h>
#include <string.h>
#include <kernel.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <x86.h>
#include <elf.h>

task_t tasks[3];
int current_task_index = 0;

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
  elf_check_supported(header);

  DEBUG("***** Preparing ELF PROCESS\r\n");

  DEBUG("ELF: Header e_type: %i\n\r",  header->e_type);
  DEBUG("ELF: Header e_machine: %i\n\r",  header->e_machine);
  DEBUG("ELF: Header e_version: %i\n\r",  header->e_version);
  DEBUG("ELF: Header e_entry: %i\n\r",  header->e_entry);
  DEBUG("ELF: Header e_phoff: %i\n\r",  header->e_phoff);
  DEBUG("ELF: Header e_shoff: %i\n\r",  header->e_shoff);
  DEBUG("ELF: Header e_flags: %i\n\r",  header->e_flags);
  DEBUG("ELF: Header e_ehsize: %i\n\r",  header->e_ehsize);
  DEBUG("ELF: Header e_phentsize: %i\n\r",  header->e_phentsize);
  DEBUG("ELF: Header e_phnum: %i\n\r",  header->e_phnum);
  DEBUG("ELF: Header e_shentsize: %i\n\r",  header->e_shentsize);
  DEBUG("ELF: Header e_shnum: %i\n\r",  header->e_shnum);
  DEBUG("ELF: Header e_shstrndx: %i\n\r",  header->e_shstrndx);
  assert(header->e_phnum == 1);

  Elf64_Phdr *prog_header = (Elf64_Phdr*)(((char*)elf_data_buffer) + header->e_phoff);
  DEBUG("ELF: P Header p_type;: 0x%X\n\r", prog_header->p_type);
  DEBUG("ELF: P Header p_flags: 0x%X\n\r", prog_header->p_flags);
  DEBUG("ELF: P Header p_offse: 0x%X\n\r", prog_header->p_offset);
  DEBUG("ELF: P Header p_vaddr: 0x%X\n\r", prog_header->p_vaddr);
  DEBUG("ELF: P Header p_paddr: 0x%X\n\r", prog_header->p_paddr);
  DEBUG("ELF: P Header p_files: 0x%X\n\r", prog_header->p_filesz);
  DEBUG("ELF: P Header p_memsz: 0x%X\n\r", prog_header->p_memsz);
  DEBUG("ELF: P Header p_align: 0x%X\n\r", prog_header->p_align);

  assert(prog_header->p_vaddr == 0);
  assert(prog_header->p_filesz <= PAGE_SIZE);
  // 1 = load - clear p_memsz bytes at p_vaddr to 0, then copy p_filesz bytes from p_offset to p_vaddr;
  void *source = ((char*)elf_data_buffer) + prog_header->p_offset;
  memset(process_buffer, 0, prog_header->p_memsz);
  memcpy(process_buffer, source, prog_header->p_filesz);

  for (int i=0; i<32; i++) {
    DEBUG("%X ", ((uint8_t*)process_buffer)[i]);
  }
  DEBUG("\n\r");

  return header->e_entry;
}


void create_user_process(task_t* task, void* elf_raw_data) {
  // setup task MMU
  uint64_t phys_addr;
  uint8_t* task_memory = alloc_frame_temp(&phys_addr);
  DEBUG("PROC: create_process_user phys_addr: 0x%X virt_addr: 0x%X\n\r", phys_addr, task_memory);

  uint64_t entry_point = load_elf(elf_raw_data, task_memory);
  
  uint64_t* rsp = (uint64_t*)(task->kstack + KERNEL_STASK_SIZE - 8);
  //rsp--;
  /*
  +------------+
  +40 | %SS        |
  +32 | %RSP       |
  +24 | %RFLAGS    |
  +16 | %CS        |
  +8 | %RIP       | <-- %RSP
  +------------+
  * */
  *rsp-- = GDT_USER_DATA | 3;              // SS
  *rsp-- = 0x200000 - 8;       // RSP  2MB - 8
  *rsp-- = 0x286;              // RFLAGS
  *rsp-- = GDT_USER_CODE | 3;              // CS
  *rsp-- = entry_point; 
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
  *rsp = 15;
  // rsp -= 14; // rax,rbx,rcx,rdx,rsi,rdi,rbp,r8,r9,r10,r11,r12,r13,r14,r15 - switch.asm irq0

  task->rsp = (uint64_t)rsp; //- switch will do it
  task->id = rand();
  task->pde.all = phys_addr | 0x87; // Present + Write + CPL3
  DEBUG("PROC: Task created : tm0: 0x%X (p:0x%X) rsp:0x%X\n\r", task_memory, phys_addr, task->rsp);
}

task_t* next_task() {
	current_task_index++;
	current_task_index = current_task_index % 2;

	return &tasks[current_task_index];
}

task_t* current_task() {
	return &tasks[current_task_index];
}

void __switch_to(task_t* next) {
  kprintf_xy(0, 0, "Task : 0x%X 0x%X", next->pde, next->rsp);
  pde_user[0] = next->pde;
  tss64.rsp0 = (uint64_t)(next->kstack + KERNEL_STASK_SIZE - 8);
  x86_tlb_flush_all();
}

void do_first_task_jump() {
  __asm__ __volatile__ ("jmp irq0_first_jump");
}



