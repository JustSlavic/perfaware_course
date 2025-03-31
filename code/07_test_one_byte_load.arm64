.global _load_1byte
.global _load_4bytes
.global _load_8bytes

.text
.align 2


_load_1byte: ; x0 - count, x1 - data
.loop0:
    ldrb    w2, [x1]
    subs    x0, x0, #1
    b.ge    .loop0
    ret

_load_4bytes: ; x0 - count, x1 - data
.loop1:
    ldr     w2, [x1]
    subs    x0, x0, #1
    b.ge    .loop1
    ret

_load_8bytes: ; x0 - count, x1 - data
.loop2:
    ldr     x2, [x1]
    subs    x0, x0, #1
    b.ge    .loop2
    ret
