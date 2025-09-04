global load_8bytes_unaligned_asm

section .text


load_8bytes_unaligned_asm: ; rcx - outer count, rdx - data,  r8 - inner count
.outer_loop:
    mov         r9, r8   ; inner loop index
    mov         rax, rdx ; current read pointer

.inner_loop:
    ; read 128 bytes
    vmovdqu     xmm0, [rax]
    vmovdqu     xmm0, [rax + 0x10]
    vmovdqu     xmm0, [rax + 0x20]
    vmovdqu     xmm0, [rax + 0x30]

    vmovdqu     xmm0, [rax + 0x40]
    vmovdqu     xmm0, [rax + 0x50]
    vmovdqu     xmm0, [rax + 0x60]
    vmovdqu     xmm0, [rax + 0x70]

    ; advance read pointer
    add         rax, 0x80
    ; inner_index -= 1, loop if (inner_index >= 0)
    sub         r9, 0x1
    jge         .inner_loop
    ; outer_index -= 1, loop if (outer_index >= 0)
    sub         rcx, 0x1
    jge         .outer_loop

    ret
