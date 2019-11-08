section .text
bits 64
global start

start:
    mov rax, 0
loop_infi:
    inc rax    
    int 0x80
    jmp loop_infi
    