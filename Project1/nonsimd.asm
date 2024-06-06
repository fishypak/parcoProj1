section .text
bits 64
default rel

global asm1d
asm1d:    
    push rsi
    push rdi
    push rbp
    mov rbp, rsp
    sub rsp, 16   ; Allocate stack space for local variables if needed

    add r8, 24            ; r8 points to the fourth element of Y (skipping the first 3)

    xor rax, rax          ; rax = 0 (index for Y)

    cmp rcx, 6            ; Check if n is greater than or equal to 6
    jl loop_end           ; If n < 6, skip the loop

loop_start:
    cmp rax, rcx
    jge loop_end          ; if rax >= rcx, exit loop

    mov r9, [rdx + rax * 8]       ; X[i]
    mov r10, [rdx + rax * 8 + 8]  ; X[i+1]
    mov r11, [rdx + rax * 8 + 16] ; X[i+2]
    mov r12, [rdx + rax * 8 + 24] ; X[i+3]
    mov r13, [rdx + rax * 8 + 32] ; X[i+4]
    mov r14, [rdx + rax * 8 + 40] ; X[i+5]
    mov r15, [rdx + rax * 8 + 48] ; X[i+6]

    add r9, r10
    add r9, r11
    add r9, r12
    add r9, r13
    add r9, r14
    add r9, r15

    mov [r8 + (rax - 3) * 8], r9        ; Y[i] = sum

    inc rax
    jmp loop_start

loop_end:
    ; Restore the stack and registers
    add rsp, 16
    pop rbp
    pop rdi
    pop rsi
    ret