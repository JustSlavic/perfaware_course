global test_branch_predictor:

section .text


test_branch_predictor:
    xor rax, rax
.loop:
    mov r10b, byte [rdx + rax]
    inc rax
    test r10, 1
    jnz .skip
    nop
.skip:
    cmp rax, rcx
    jb .loop
    ret
