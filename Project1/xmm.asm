section .data
    reset dq 0.0

section .text
bits 64
default rel
global xmm1D
xmm1D:
    ; Save registers and set up the stack frame
    push rsi
    push rdi
    push rbp
    mov rbp, rsp
    sub rsp, 16   ; Allocate stack space for local variables if needed

    sub rsp,0x10
	movdqu [rsp],xmm6

    ; Initialize counters and pointers
    mov rdi, rcx      ; Store the original ARRAY_SIZE
    sub rdi, 1        ; Adjust for accessing up to i+3
    xor rax, rax      ; Loop counter initialized

main_loop:
    ; Ensure we do not go beyond array bounds
    cmp rax, rdi
    jge loop_end

    ; Load elements with XMM registers for 64-bit values
    movdqu xmm0, [rdx + rax * 8]       ; Load X[i-3]
    movdqu xmm1, [rdx + rax * 8 + 8]   ; Load X[i-2]
    movdqu xmm2, [rdx + rax * 8 + 16]  ; Load X[i-1]
    movdqu xmm3, [rdx + rax * 8 + 24]  ; Load X[i]
    movdqu xmm4, [rdx + rax * 8 + 32]  ; Load X[i+1]
    movdqu xmm5, [rdx + rax * 8 + 40]  ; Load X[i+2]
    movdqu xmm6, [rdx + rax * 8 + 48]  ; Load X[i+3]

    ; Sum the values in the stencil using packed add (paddq for 64-bit integers)
    paddq xmm0, xmm1
    paddq xmm0, xmm2
    paddq xmm0, xmm3
    paddq xmm0, xmm4
    paddq xmm0, xmm5
    paddq xmm0, xmm6

    ; Store result to output vector
    movdqu [r8 + rax * 8], xmm0

    ; Increment to process the next group of elements
    add rax, 1
    jmp main_loop

loop_end:
    ; Restore the stack and registers
    movdqu xmm6,[rsp]
	add rsp,0x10
    add rsp, 16
    pop rbp
    pop rdi
    pop rsi
    ret