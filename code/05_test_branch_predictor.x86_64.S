global test_branch_predictor_asm

section .text


test_branch_predictor_asm:
    xor     rax, rax
.loop:
    mov     r10b, byte [rdx + rax]
    inc     rax
    test    r10, 1
    jnz     .skip
    nop
.skip:
    cmp     rax, rcx
    jl      .loop
    ret
