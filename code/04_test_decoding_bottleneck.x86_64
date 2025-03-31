global nop_1x3byte_asm
global nop_3x1byte_asm
global nop_9x1byte_asm

section .text

nop_1x3byte_asm:
    xor     rax, rax
.loop0:
    db      0x0f, 0x1f, 0x00
    inc     rax
    cmp     rax, rcx
    jb      .loop0
    ret

nop_3x1byte_asm:
    xor     rax, rax
.loop1:
    nop
    nop
    nop
    inc     rax
    cmp     rax, rcx
    jb      .loop1
    ret

nop_9x1byte_asm:
    xor     rax, rax
.loop2:
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    inc     rax
    cmp     rax, rcx
    jb      .loop2
    ret
