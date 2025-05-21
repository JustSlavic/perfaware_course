.global _load_8bytes_unaligned_asm

.text
.align 2


_load_8bytes_unaligned_asm: ; x0 - outer count, x1 - data, x2 - inner count
.outer_loop:
    mov     x3, x2 ; inner loop index
    mov     x4, x1 ; current read pointer

.inner_loop:
    ; read 128 bytes
    ldr     q0, [x4]
    ldr     q0, [x4, #0x10]
    ldr     q0, [x4, #0x20]
    ldr     q0, [x4, #0x30]

    ldr     q0, [x4, #0x40]
    ldr     q0, [x4, #0x50]
    ldr     q0, [x4, #0x60]
    ldr     q0, [x4, #0x70]

    ; advance read pointer
    add     x4, x4, #0x80
    ; inner_index -= 1, loop if (inner_index >= 0)
    subs    x3, x3, #1
    b.ge    .inner_loop
    ; outer_index -= 1, loop if (outer_index >= 0)
    subs    x0, x0, #1
    b.ge    .outer_loop

    ret


; _load_8bytes_unaligned_asm: ; x0 - count, x1 - data, x2 - offset
;     mov     x3, xzr ; x3 - index
;     mov     x6, xzr ; x6 - result
; .loop2:
;     add     x4, x1, x3     ; p = data + index
;     ldr     x5, [x4]   ; *(uint64 *)(p + 4) ; read 8 bytes with 4 bytes off
;     add     x6, x6, #8
;     add     x3, x3, x2     ; index += offset
;     cmp     x3, x0
;     b.lt    .loop2
;     mov     x0, x6
;     ret
