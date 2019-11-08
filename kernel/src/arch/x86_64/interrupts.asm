global interrupt
extern isr_handler
extern irq_handler

section .text
bits 64

%macro isr_stub_err 1
global isr_stub_%1
isr_stub_%1:
    cli
    ; Store the interrupt number in the highest four bytes of the error code
    ; This way we can always increment rsp by 8 before iretq and no memory is wasted.
    mov dword [rsp + 4], %1
    jmp isr_common_stub
%endmacro

%macro isr_stub_noerr 1
global isr_stub_%1
isr_stub_%1:
    cli
    ; For consistency with the err variant
    push qword 0
    mov dword [rsp + 4], %1
    jmp isr_common_stub
%endmacro

%macro isr_stub_irq 1
global isr_stub_%1
isr_stub_%1:
    cli
    push qword 0
    mov dword [rsp + 4], %1
    jmp isr_common_stub
%endmacro

%macro isr_stub_nop 1
global isr_stub_%1
isr_stub_%1:
    ; TODO: Should we call the handler anyway?
    iretq
%endmacro

%macro isr_stub_ipi 2
global isr_stub_%1
isr_stub_%1:
    cli
    push qword %1
    jmp isr_common_stub
%endmacro

isr_common_stub:
    ; save registers
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
    
    ; call handler
    mov rdi, rsp
    call isr_handler

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
    pop rax

    add rsp, 8     ; Cleans up the pushed error code and pushed ISR number

    sti
    iretq

; define interruptions
; should be keep in sync with src/core/isr.h
;; -----------------------------------------------
;; STANDARD INTTERUPTS/TRAPS
isr_stub_noerr 0
isr_stub_noerr 1
isr_stub_noerr 2
isr_stub_noerr 3
isr_stub_noerr 4
isr_stub_noerr 5
isr_stub_noerr 6
isr_stub_noerr 7
isr_stub_err 8
isr_stub_noerr 9
isr_stub_err 10
isr_stub_err 11
isr_stub_err 12
isr_stub_err 13
isr_stub_err 14
isr_stub_noerr 15
isr_stub_noerr 16
isr_stub_err 17
isr_stub_noerr 18
isr_stub_noerr 19
isr_stub_noerr 20
isr_stub_noerr 21
isr_stub_noerr 22
isr_stub_noerr 23
isr_stub_noerr 24
isr_stub_noerr 25
isr_stub_noerr 26
isr_stub_noerr 27
isr_stub_noerr 28
isr_stub_noerr 29
isr_stub_err 30
isr_stub_noerr 31

;; ----------------------------------------------
;; IRQ SECTION
;; IRQ 0 - F (32 - 47)
;isr_stub_irq 32 - scheduler
isr_stub_irq 33 ; -- keyboard
isr_stub_irq 34 
isr_stub_irq 35
isr_stub_irq 36
isr_stub_irq 37
isr_stub_irq 38
isr_stub_irq 39
isr_stub_irq 40
isr_stub_irq 41
isr_stub_irq 42
isr_stub_irq 43
isr_stub_irq 44
isr_stub_irq 45 ; -- mouse
isr_stub_irq 46
isr_stub_irq 47

;; -----------------------------------------------
;; SYSCALL SECTION
; isr_stub_noerr 128 - syscall
