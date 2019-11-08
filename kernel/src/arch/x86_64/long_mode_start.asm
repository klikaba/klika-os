%define GDT_KERNEL_DATA 0x10
%define GDT_TSS         0x28 

global long_mode_start
global load_tss
extern kmain

extern pml4e_table
extern kernel_stack_top
extern gdt64
extern gdt64.tss
extern gdt64.pointer
extern tss64
extern tss64.rsp0

%define KERNEL_VMA          0xFFFF800000000000

section .text
bits 64

long_mode_start:
    cli
    ; Now we are using two mappings PML4E 0 and x0100 
    ; we want to remove lower mapping on zero. To do that
    ; we need to flush RIP with 64bit higher address (FFFF800000...) 
    ; and also to reset GDT with higher address 
    mov rax, flush_rip 
    jmp rax
flush_rip:

    mov rax, gdt64.pointer
    lgdt [rax]

    ; reset segments registers jun in case (in case of what?)
    mov ax, GDT_KERNEL_DATA
    mov ss, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; setup new stack
    mov rsp, kernel_stack_top
    mov rax, tss64.rsp0
    mov [rax], rsp

    ; enable 64bit TSS
    mov rdi, gdt64
    add rdi, gdt64.tss
    mov rax, tss64
    mov word [rdi + 2], ax
    shr rax, 16
    mov byte [rdi + 4], al
    shr rax, 8
    mov byte [rdi + 7], al
    shr rax, 8
    mov dword [rdi + 8], eax

    mov ax, GDT_TSS
    ltr ax

    ; GRUB stores a pointer to a struct in the register ebx that,
    ; among other things, describes at which addresses the modules are loaded.
    ; Push ebx on the stack before calling main to make it an argument for main.
    call kmain
    hlt

