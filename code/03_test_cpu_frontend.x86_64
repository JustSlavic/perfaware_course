global write_buffer_asm
global nop_buffer_asm
global cmp_buffer_asm
global dec_buffer_asm

section .text

write_buffer_asm:
    xor     rax, rax
.loop0:
    mov     [rdx + rax], al
    inc     rax
    cmp     rax, rcx
    jb      .loop0
    ret

nop_buffer_asm:
    xor     rax, rax
.loop1:
    db      0x0f, 0x1f, 0x00
    inc     rax
    cmp     rax, rcx
    jb      .loop1
    ret


cmp_buffer_asm:
    xor     rax, rax
.loop2:
    inc     rax
    cmp     rax, rcx
    jb      .loop2
    ret


dec_buffer_asm:
.loop3:
    dec     rcx
    jnz     .loop3
    ret



