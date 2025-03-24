global write_buffer_asm
global noop_buffer_asm
global cmp_buffer_asm
global dec_buffer_asm

section .text

write_buffer_asm:
    xor rax, rax
.loop:
    mov [rdx + rax], al
    inc rax
    cmp rax, rcx
    jb .loop
    ret

noop_buffer_asm:
    xor rax, rax
.loop:
    db 0x0f, 0x1f, 0x00
    inc rax
    cmp rax, rcx
    jb .loop
    ret


cmp_buffer_asm:
    xor rax, rax
.loop:
    inc rax
    cmp rax, rcx
    jb .loop
    ret


dec_buffer_asm:
    xor rax, rax
.loop:
    dec rcx
    jnz .loop
    ret



