global load_1x8bytes
global load_2x8bytes
global load_3x8bytes
global load_4x8bytes
global store_1x8bytes
global store_2x8bytes
global store_3x8bytes
global store_4x8bytes

section .text


load_1x8bytes: ; rcx - count, rdx - data
.loop0:
    mov     rax, [rdx]
    sub     rcx, 8
    jge     .loop0
    ret

load_2x8bytes: ; rcx - count, rdx - data
.loop1:
    mov     rax, [rdx]
    mov     rax, [rdx]
    sub     rcx, 16
    jge     .loop1
    ret

load_3x8bytes: ; rcx - count, rdx - data
.loop2:
    mov     rax, [rdx]
    mov     rax, [rdx]
    mov     rax, [rdx]
    sub     rcx, 24
    jge     .loop2
    ret

load_4x8bytes: ; rcx - count, rdx - data
.loop3:
    mov     rax, [rdx]
    mov     rax, [rdx]
    mov     rax, [rdx]
    mov     rax, [rdx]
    sub     rcx, 32
    jge     .loop3
    ret

store_1x8bytes: ; rcx - count, rdx - data
.loop4:
    mov     [rdx], rax
    sub     rcx, 8
    jge     .loop4
    ret

store_2x8bytes: ; rcx - count, rdx - data
.loop5:
    mov     [rdx], rax
    mov     [rdx], rax
    sub     rcx, 16
    jge     .loop5
    ret

store_3x8bytes: ; rcx - count, rdx - data
.loop6:
    mov     [rdx], rax
    mov     [rdx], rax
    mov     [rdx], rax
    sub     rcx, 24
    jge     .loop6
    ret

store_4x8bytes: ; rcx - count, rdx - data
.loop7:
    mov     [rdx], rax
    mov     [rdx], rax
    mov     [rdx], rax
    mov     [rdx], rax
    sub     rcx, 32
    jge     .loop7
    ret
