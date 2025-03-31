.global _load_1x8bytes
.global _load_2x8bytes
.global _load_3x8bytes
.global _load_4x8bytes
.global _store_1x8bytes
.global _store_2x8bytes
.global _store_3x8bytes
.global _store_4x8bytes

.text
.align 2


_load_1x8bytes: ; x0 - count, x1 - data
.loop0:
    ldr     x2, [x1]
    subs    x0, x0, #8
    b.ge    .loop0
    ret

_load_2x8bytes: ; x0 - count, x1 - data
.loop1:
    ldr     x2, [x1]
    ldr     x2, [x1]
    subs    x0, x0, #16
    b.ge    .loop1
    ret

_load_3x8bytes: ; x0 - count, x1 - data
.loop2:
    ldr     x2, [x1]
    ldr     x2, [x1]
    ldr     x2, [x1]
    subs    x0, x0, #24
    b.ge    .loop2
    ret

_load_4x8bytes: ; x0 - count, x1 - data
.loop3:
    ldr     x2, [x1]
    ldr     x2, [x1]
    ldr     x2, [x1]
    ldr     x2, [x1]
    subs    x0, x0, #32
    b.ge    .loop3
    ret

_store_1x8bytes: ; x0 - count, x1 - data
.loop4:
    str     x2, [x1]
    subs    x0, x0, #8
    b.ge    .loop4
    ret

_store_2x8bytes: ; x0 - count, x1 - data
.loop5:
    str     x2, [x1]
    str     x2, [x1]
    subs    x0, x0, #16
    b.ge    .loop5
    ret

_store_3x8bytes: ; x0 - count, x1 - data
.loop6:
    str     x2, [x1]
    str     x2, [x1]
    str     x2, [x1]
    subs    x0, x0, #24
    b.ge    .loop6
    ret

_store_4x8bytes: ; x0 - count, x1 - data
.loop7:
    str     x2, [x1]
    str     x2, [x1]
    str     x2, [x1]
    str     x2, [x1]
    subs    x0, x0, #32
    b.ge    .loop7
    ret
