.global _load_3x4bytes
.global _load_3x8bytes
.global _load_3x32bytes

.text
.align 2

_load_3x4bytes: ; x0 - count, x1 - data
.loop0:
    ldr     w2, [x1]
    ldr     w2, [x1, #4]
    ldr     w2, [x1, #8]
    subs    x0, x0, #12
    b.ge    .loop0
    ret

_load_3x8bytes: ; x0 - count, x1 - data
.loop1:
    ldr     x2, [x1]
    ldr     x2, [x1, #8]
    ldr     x2, [x1, #16]
    subs    x0, x0, #24
    b.ge    .loop1
    ret

_load_3x32bytes: ; x0 - count, x1 - data
.loop2:
    ldr     q2, [x1]
    ldr     q2, [x1, #16]
    ldr     q2, [x1, #32]
    subs    x0, x0, #48
    b.ge    .loop2
    ret
