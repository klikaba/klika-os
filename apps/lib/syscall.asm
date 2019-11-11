section .text
bits 64

global syscall
syscall:
	mov rax, rdi			;Syscall number -> rax.  
	mov rdi, rsi			; shift arg1 - arg5.
	mov rsi, rdx
	mov rdx, rcx
	mov rcx, r8
	mov r8,  r9
	int 0x80					; Do the system call.  
	ret								; Return to caller.  