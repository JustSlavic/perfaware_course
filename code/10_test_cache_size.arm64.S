.global _test_cache_size_asm

.text
.align 2

_test_cache_size_asm: ; x0 - size, x1 - data, x2 - mask
    ; x3 - pointer
    ; x4 - offset
    mov     x3, x1
    mov     x4, xzr

.loop:
    ; Read 256 bytes
    ldr     q0, [x3]
    ldr     q0, [x3, #0x10]
    ldr     q0, [x3, #0x20]
    ldr     q0, [x3, #0x30]

    ldr     q0, [x3, #0x40]
    ldr     q0, [x3, #0x50]
    ldr     q0, [x3, #0x60]
    ldr     q0, [x3, #0x70]

    ldr     q0, [x3, #0x80]
    ldr     q0, [x3, #0x90]
    ldr     q0, [x3, #0xa0]
    ldr     q0, [x3, #0xb0]

    ldr     q0, [x3, #0xc0]
    ldr     q0, [x3, #0xd0]
    ldr     q0, [x3, #0xe0]
    ldr     q0, [x3, #0xf0]

    ; Advance and mask the read offset
    add     x4, x4, #0x100
    and     x4, x4, x2

    ; Update the read base pointer to point to the new offset
    mov     x3, x1
    add     x3, x3, x4

    ; Repeat
    subs    x0, x0, #0x100
    b.ge    .loop

    ret

