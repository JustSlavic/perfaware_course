global load_2x4bytes
global load_2x8bytes
global load_2x16bytes
global load_2x32bytes
; global load_2x64bytes

section .text


load_2x4bytes: ; count - rcx, data - rdx
.loop0:
    mov     eax, [rdx]
    mov     eax, [rdx + 4]
    sub     rcx, 8
    jnle    .loop0
    ret

load_2x8bytes: ; count - rcx, data - rdx
.loop1:
    mov     rax, [rdx]
    mov     rax, [rdx + 8]
    sub     rcx, 16
    jnle    .loop1
    ret

load_2x16bytes: ; count - rcx, data - rdx
.loop2:
    vmovdqu xmm0, [rdx]
    vmovdqu xmm0, [rdx + 16]
    sub     rcx, 32
    jnle    .loop2
    ret

load_2x32bytes: ; count - rcx, data - rdx
.loop3:
    vmovdqu ymm0, [rdx]
    vmovdqu ymm0, [rdx + 32]
    sub     rcx, 64
    jnle    .loop3
    ret

; load_2x64bytes:
; .loop4:
;     vmovdqu zmm0, [rdx]
;     vmovdqu zmm0, [rdx]
;     sub rcx, 128
;     jnle .loop4
;     ret
