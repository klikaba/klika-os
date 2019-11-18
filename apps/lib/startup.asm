%define STACK_SIZE 4096*4

extern main
extern init_heap
global start
global user_heap_start

section .text
bits 64
start:
	; setup stack
	mov rsp, user_stack_top

	; setup heap
	call init_heap

	; jump into user program
	call main
end:
	jmp end

section .bss
align 4
    resb STACK_SIZE
user_stack_top:

