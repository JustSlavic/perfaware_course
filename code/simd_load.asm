global mmx_available
global sse_available
global sse2_available
global sse3_available
global ssse3_available
global sse4_1_available
global sse4_2_available

global test_load_2x32
global test_load_2x64
global test_load_2x128
global test_load_2x256
; global test_load_2x512

section .text


test_load_2x32: ; count - rcx, data - rdx
    align 64
.loop:
    mov eax, [rdx]
    mov eax, [rdx + 4]
    sub rcx, 8
    jnle .loop
    ret

test_load_2x64: ; count - rcx, data - rdx
    align 64
.loop:
    mov rax, [rdx]
    mov rax, [rdx + 8]
    sub rcx, 16
    jnle .loop
    ret

test_load_2x128: ; count - rcx, data - rdx
    align 64
.loop:
    vmovdqu xmm0, [rdx]
    vmovdqu xmm0, [rdx + 16]
    sub rcx, 32
    jnle .loop
    ret

test_load_2x256:
    align 64
.loop:
    vmovdqu ymm0, [rdx]
    vmovdqu ymm0, [rdx + 32]
    sub rcx, 64
    jnle .loop
    ret

; MMX Technology support
mmx_available:
    mov eax, 1
    cpuid
    mov eax, edx
    shr eax, 23  ; 23-th bit in edx is the mmx flag
    and eax, 1
    ret

; Streaming SIMD Extension
sse_available:
    mov eax, 1
    cpuid
    mov eax, edx
    shr eax, 25  ; 25-th bit in edx is the sse flag
    and eax, 1
    ret

; Streaming SIMD Extension 2
sse2_available:
    mov eax, 1
    cpuid
    mov eax, edx
    shr eax, 26  ; 26-th bit in edx is the sse2 flag
    and eax, 1
    ret

; Streaming SIMD Extension 3
sse3_available:
    mov eax, 1
    cpuid
    mov eax, ecx
    and eax, 1    ; 0-th bit in ecx is the ees3 flag
    ret

; SUpplemental Streaming SIMD Extension 3
ssse3_available:
    mov eax, 1
    cpuid
    mov eax, ecx
    shr eax, 9    ; 9-th bit in ecx is the ssse3 flag
    and eax, 1
    ret

; SSE4.1
sse4_1_available:
    mov eax, 1
    cpuid
    mov eax, ecx
    shr eax, 19   ; 19-th bit in ecx is the sse4.1 flag
    and eax, 1
    ret

; SSE4.2
sse4_2_available:
    mov eax, 1
    cpuid
    mov eax, ecx
    shr eax, 20   ; 20-th bit in ecx is the sse4.2 flag
    and eax, 1
    ret



; test_load_2x512:
;     align 64
; .loop:
;     vmovdqu zmm0, [rdx]
;     vmovdqu zmm0, [rdx]
;     sub rcx, 128
;     jnle .loop
;     ret
