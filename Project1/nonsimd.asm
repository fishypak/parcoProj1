section .data
    reset dq 0.0

section .text
bits 64 ; Specify that we are writing 64-bit code
default rel

global asm1d
asm1d:	
    push rbp
    mov rbp, rsp
    sub rsp, 16           ; Allocate stack space for local variables

    mov rbx, rdi          ; rbx = n (length of the vector)
    mov rdx, rsi          ; rdx = address of X
    mov rdi, rsi          ; rdi = address of Y
    add rdi, 24           ; rdi points to the fourth element of Y (skipping the first 3)

    xor rax, rax          ; rax = 0 (index for Y)

    cmp rbx, 6            ; Check if n is greater than or equal to 6
    jl loop_end           ; If n < 6, skip the loop

    sub rbx, 3            ; Exclude the last three elements
    dec rbx               ; Exclude the first element

loop_start:
    cmp rax, rbx
    jge loop_end          ; if rax >= n, exit loop

    mov r8, [rdx + rax * 8]       ; X[i]
    mov r9, [rdx + rax * 8 + 8]   ; X[i+1]
    mov r10, [rdx + rax * 8 + 16] ; X[i+2]
    mov r11, [rdx + rax * 8 + 24] ; X[i+3]
    mov r12, [rdx + rax * 8 + 32] ; X[i+4]
    mov r13, [rdx + rax * 8 + 40] ; X[i+5]

    add r8, r9
    add r8, r10
    add r8, r11
    add r8, r12
    add r8, r13
    add r8, r14

    mov [rdi + rax * 8], r8        ; Y[i] = sum

    inc rax
    jmp loop_start

loop_end:
    add rsp, 16
    pop rbp
    ret