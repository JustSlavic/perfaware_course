.global _nop_1x4byte_asm
.global _nop_2x4byte_asm
.global _nop_4x4byte_asm
.global _nop_8x4byte_asm

.text
.align 2

_nop_1x4byte_asm: ; x0 - count
    mov     x2, xzr      ; x2 - index
.loop0:
    nop
    add     x2, x2, #1
    cmp     x2, x0
    b.cc    .loop0
    ret

_nop_2x4byte_asm: ; x0 - count
    mov     x2, xzr      ; x2 - index
.loop1:
    nop
    nop
    add     x2, x2, #1
    cmp     x2, x0
    b.cc    .loop1
    ret

_nop_4x4byte_asm: ; x0 - count
    mov     x2, xzr      ; x2 - index
.loop2:
    nop
    nop
    nop
    nop
    add     x2, x2, #1
    cmp     x2, x0
    b.cc    .loop2
    ret

_nop_8x4byte_asm: ; x0 - count
    mov     x2, xzr      ; x2 - index
.loop3:
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    add     x2, x2, #1
    cmp     x2, x0
    b.cc    .loop3
    ret
