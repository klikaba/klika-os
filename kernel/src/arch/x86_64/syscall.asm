; ----------------------------------------------------------------
extern system_call_handler
global isr_stub_128

isr_stub_128:
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

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax    

    ; call handler
    mov rdi, rsp
    call system_call_handler

    ; restore registers
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
    ;pop rax
    add rsp, 8 ; skip pushed rax - return result fo system_call_handler

    sti
    iretq

