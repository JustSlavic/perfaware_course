.global _rat_add_asm
.global _rat_mov_add_asm

.text
.align 2


_rat_add_asm: ; x0 - count
.loop0:
    add     x1, x1, #1
    add     x1, x1, #1
    subs    x0, x0, #1
    b.ge    .loop0
    ret

_rat_mov_add_asm: ; x0 - count
.loop1:
    mov     x2, x1
    add     x2, x2, #1
    mov     x2, x1
    add     x2, x2, #1
    subs    x0, x0, #1
    b.ge    .loop1
    ret
