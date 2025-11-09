global load_1byte
global load_4bytes
global load_8bytes

section .text

load_1byte: ; rcx - count, rdx - data
.loop0:
    mov     al, [rdx]
    dec     rcx
    jnz     .loop0
    ret

load_4bytes: ; rcx - count, rdx - data
.loop1:
    mov     eax, [rdx]
    dec     rcx
    jnz     .loop1
    ret

load_8bytes: ; rcx - count, rdx - data
.loop2:
    mov     rax, [rdx]
    dec     rcx
    jnz     .loop2
    ret
