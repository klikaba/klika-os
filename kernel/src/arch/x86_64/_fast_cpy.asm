section .text

global fast_memcpy
bits 64

;                 rdi    rsi  rdx
; fast_memcpy(*dst, *src, size)
fast_memcpy:
    mov rax, rdi
    mov rcx, rdx
    shr rcx, 3
    and edx, 7
    rep movsq
    mov ecx, edx
    rep movsb
    ret
