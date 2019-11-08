nasm -w-number-overflow -fbin dummy.asm -o dummy.app
xxd -i dummy.app > dummy_array.c
