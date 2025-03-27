global test_cache_size

section .text

test_cache_size_: ; count - RCX, data - RDX, mask - R8
    align 64
    xor rax, rax
.loop:
    ; read data
    vmovdqu ymm0, [rdx + rax]
    vmovdqu ymm0, [rdx + rax + 20h]
    ; advance index (rax)
    add rax, 40h
    and rax, r8
    ; loop maintanance
    sub rcx, 40h
    jnle .loop

    ret


test_cache_size: ; count - RCX, data - RDX, mask - R8
    xor r9, r9
    mov rax, rdx
    align 64

.loop:
    ; Read 256 bytes
    vmovdqu ymm0, [rax]
    vmovdqu ymm0, [rax + 0x20]
    vmovdqu ymm0, [rax + 0x40]
    vmovdqu ymm0, [rax + 0x60]
    vmovdqu ymm0, [rax + 0x80]
    vmovdqu ymm0, [rax + 0xa0]
    vmovdqu ymm0, [rax + 0xc0]
    vmovdqu ymm0, [rax + 0xe0]

    ; Advance and mask the read offset
    add r9, 0x100
    and r9, r8

    ; Update the read base pointer to point to the new offset
    mov rax, rdx
    add rax, r9

    ; Repeat
    sub rcx, 0x100
    jnz .loop

    ret
