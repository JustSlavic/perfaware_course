.global _write_buffer_asm
.global _nop_buffer_asm
.global _cmp_buffer_asm
.global _dec_buffer_asm

.text
.align 2

_write_buffer_asm:       ; x0 - count, x1 - data
    mov     x2, xzr      ; x2 - index
.loop0:
    strb    w2, [x1, x2]
    add     x2, x2, #1
    cmp     x2, x0
    b.cc    .loop0
    ret

_nop_buffer_asm: ; x0 - count, x1 - data
    mov     x2, xzr
.loop1:
    nop ; @note: remember that arm assembly is fixed-sized encoding
        ;        so we don't have to adjust the size of the nop
    add     x2, x2, #1
    cmp     x2, x0
    b.cc    .loop1
    ret

_cmp_buffer_asm: ; x0 - count, x1 - data
    mov     x2, xzr
.loop2:
    add     x2, x2, #1
    cmp     x2, x0
    b.cc    .loop2
    ret

_dec_buffer_asm: ; x0 - count, x1 - data
.loop3:
    sub     x0, x0, #1
    cmp     x0, xzr
    b.ge    .loop3
    ret
