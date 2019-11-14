; ----------------------------------------------------------------
; - TASK SWITCH
%define GDT_KERNEL_DATA   0x10
%define GDT_USER_DATA   0x20

extern next_task
extern timer_callback
extern current_task_index
extern current_task
extern __switch_to

global isr_stub_32
global irq0_first_jump


isr_stub_32:
    cli
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    ; Store rsp of current task
    call current_task
    mov [rax], rsp
irq0_first_jump:
    call next_task
    mov rsp, [rax]

    mov rdi, rax
    call __switch_to

    call timer_callback

    ; PIC : End of innterrupt
	mov al, 0x20
	out 0x20, al

    ; mov ax, GDT_USER_DATA | 3
    ; mov ds, ax
    ; mov es, ax
    ; mov fs, ax
    ; mov gs, ax

    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax
    sti
    iretq
