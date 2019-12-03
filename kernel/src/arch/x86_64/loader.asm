extern long_mode_start

%define KERNEL_VMA          0xFFFF800000000000
%define PAGE_SIZE           4096
%define KERNEL_STACK_SIZE   PAGE_SIZE*2

section .text
bits 32
global start
start:
    ; Note: All addressing before long jump to long_mode_start MUST be
    ; relative - substract KERNEL_VMA from all adresses
    
    ; save eax and ebx (grub magic and mb info) into esi and edi
    ; per GCC C ABI param specs 
    ; as it will be passed down to kernel main as arguments
    mov esi, ebx
    mov edi, eax

    ; setup stack
    mov esp, (kernel_stack_top - KERNEL_VMA)

    ; setup page entries to map kernel from KERNEL_VMA up to 2MB
    call set_up_page_tables
    ; setup cr4 and cr3 to enable paging
    call enable_paging

    ; load the 64-bit GDT
    lgdt [(gdt64.pointer - KERNEL_VMA)]

    ; jump to refresh RIP and new GDT
    jmp gdt64.kernel_code:(long_mode_start - KERNEL_VMA)

; Maps memory from 0x0000 -> 2MB and KERNEL_VMA ->2MB
set_up_page_tables:
    ; map 0th P4 entry to P3 table
    mov eax, (p3_table - KERNEL_VMA)
    or eax, 0b11 ; present + writable
    mov [(p4_table - KERNEL_VMA)], eax

    ; and 0x100th to KERNEL_VMA
    mov ecx, 0x100
    mov [(p4_table - KERNEL_VMA) + ecx * 8], eax

    ; map first P3 entry to P2 table
    mov eax, (p2_table - KERNEL_VMA)
    or eax, 0b11 ; present + writable
    mov [(p3_table - KERNEL_VMA)], eax

    ; coutner or 2MB tables
    mov ecx, 0
.map_p2_table:
    mov eax, 0x200000      ; 2MB page
    mul ecx            ; start address of ecx-th page
    or eax, 0b10000011 ; present + writable + huge
    mov [(p2_table - KERNEL_VMA) + ecx * 8], eax ; map ecx-th entry

    inc ecx            ; increase counter
    cmp ecx, 512       ; if counter == 512, the whole P2 table is mapped
    jne .map_p2_table  ; else map the next entry

    ret

enable_paging:
    ; load P4 to cr3 register (cpu uses this to access the P4 table)
    mov eax, (p4_table - KERNEL_VMA)
    mov cr3, eax

    ; enable Physical Address Extension (PAE-flag) in cr4
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    ; set the long mode bit in the EFER MSR (model specific register)
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    ; enable paging in the cr0 register
    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax

    ret


section .data
align 4096

global p4_table
p4_table:
    times 512 dq 0 ; 0 temporary to p3_table
global p3_table
p3_table:
    times 512 dq 0 ; 0th point to p2_table
global p2_table
p2_table: 
    times 512 dq 0 ; 0th points to page
global tmp_mem_bitmap
tmp_mem_bitmap:
    times 512 dq 0 ; temporary mem table 1bit = 4K total of 128MB

global tss64
global tss64.rsp0
tss64:
    dd 0
tss64.rsp0:
    times 3 dq 0 ; RSPn
    dq 0 ; Reserved
interrupt_stack_table:
    dq ist_stack_1 ; IST1, NMI
    dq ist_stack_2 ; IST2, Double fault
    dq 0 ; IST3
    dq 0 ; IST4
    dq 0 ; IST5
    dq 0 ; IST6
    dq 0 ; IST7
    dq 0 ; Reserved
    dw 0 ; Reserved
    dw 0 ; I/O Map Base Address
tss_size equ $ - tss64 - 1


section .data
global gdt64
global gdt64.tss
global gdt64.pointer
gdt64:                           ; Global Descriptor Table (64-bit).
.null: equ $ - gdt64         ; The null descriptor.
    dw 0                         ; Limit (low).
    dw 0                         ; Base (low).
    db 0                         ; Base (middle)
    db 0                         ; Access.
    db 0                         ; Granularity.
    db 0                         ; Base (high).
.kernel_code: equ $ - gdt64         ; The code descriptor.
    dw 0                         ; Limit (low).
    dw 0                         ; Base (low).
    db 0                         ; Base (middle)
    db 10011000b                 ; Present=1 + DPL=00 + S=1 (system segment) + Type=1000(Execute only)
    db 00100000b                 ; Granularity, 64 bits flag, limit19:16.
    db 0                         ; Base (high).
.kernel_data: equ $ - gdt64         ; The data descriptor.
    dw 0                         ; Limit (low).
    dw 0                         ; Base (low).
    db 0                         ; Base (middle)
    db 10010010b                 ; Present=1 + DPL=00 + S=1 + Type=0010(Read/Write)
    db 00000000b                 ; Granularity.
    db 0                         ; Base (high).
.user_code: equ $ - gdt64         ; The code descriptor.
    dw 0                         ; Limit (low).
    dw 0                         ; Base (low).
    db 0                         ; Base (middle)
    db 11111000b                 ; Present=1 + DPL=00 + S=1 (system segment) + Type=1000(Execute only)
    db 00100000b                ; Granularity, 64 bits flag, limit19:16.
    db 0                         ; Base (high).
.user_data: equ $ - gdt64         ; The data descriptor.
    dw 0                         ; Limit (low).
    dw 0                         ; Base (low).
    db 0                         ; Base (middle)
    db 11110010b                 ; Present=1 + DPL=00 + S=1 + Type=0010(Read/Write)
    db 00000000b                 ; Granularity.
    db 0                         ; Base (high).
.tss equ $ - gdt64           ; The TSS descriptor
    dw tss_size & 0xFFFF         ; Limit
    dw 0                         ; Base (bytes 0-2)
    db 0                         ; Base (byte 3)
    db 10001001b                 ; Type, present
    db 00000000b                 ; Misc
    db 0                         ; Base (byte 4)
    dd 0                         ; Base (bytes 5-8)
    dd 0                         ; Zero / reserved
.pointer:                    ; The GDT-pointer.
    dw $ - gdt64 - 1             ; Limit.
    dq gdt64                     ; Base.

section .bss
align 4
kernel_stack_bottom:
    resb KERNEL_STACK_SIZE
global kernel_stack_top
kernel_stack_top:
    resb PAGE_SIZE
ist_stack_1:
    resb PAGE_SIZE
ist_stack_2:
