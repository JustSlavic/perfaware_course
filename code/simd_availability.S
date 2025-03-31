global mmx_available
global sse_available
global sse2_available
global sse3_available
global ssse3_available
global sse4_1_available
global sse4_2_available

section .text


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
