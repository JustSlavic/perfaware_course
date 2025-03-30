.global _test_branch_predictor_asm

.text
.align 2


_test_branch_predictor_asm: ; x0 - count, x1 - data
    mov     x2, xzr ; x2 - index
.loop:
    ldrb    w3, [x1, x2]
    add     x2, x2, #1
    ands    x3, x3, #1
    b.ne    .skip
    nop
.skip:
    cmp     x2, x0
    b.cc    .loop
    ret
