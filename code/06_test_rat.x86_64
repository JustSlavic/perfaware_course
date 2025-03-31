global rat_add_asm
global rat_mov_add_asm

section .text

rat_add_asm:
.loop0:
    add     rax, 1
    add     rax, 1
    dec     rcx
    jnz     .loop0
    ret

rat_mov_add_asm:
.loop1:
    mov     rax, rcx
    add     rax, 1
    mov     rax, rcx
    add     rax, 1
    dec     rcx
    jnz     .loop1
    ret
