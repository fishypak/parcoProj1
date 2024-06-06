section .text
bits 64
default rel
global ymm1d

; RCX - ARRAY_SIZE
; RDX - Address of input vector x -> [rdx] is value of x 
; R8 - Address of output vector y -> [r8] is value of y

ymm1d:
    ; Save registers and set up the stack frame
    push rsi
    push rdi
    push rbp
    mov rbp, rsp
    sub rsp, 16   ; Allocate stack space for local variables if needed

    ; Number of times to loop
    shr rcx, 2              ; 
    sub rcx, 1              ; fix boundary check, going out of bounds sometimes if not divisible by 4
    mov rdi, rcx
    xor rax, rax          ; rax = 0 (index for Y)


    ; Perform stencil
    stencil:
        cmp rax, rdi
        jge end          ; if rax >= rcx, exit loop

        ; Move values to registers
        vmovdqu ymm1, [rdx]     
        vmovdqu ymm2, [rdx + 8]  
        vmovdqu ymm3, [rdx + 16]  
        vmovdqu ymm4, [rdx + 24] 
        vmovdqu ymm5, [rdx + 32] 
        vmovdqu ymm6, [rdx + 40] 
        vmovdqu ymm7, [rdx + 48] 

        ; sum
        vpaddq  ymm1, ymm1, ymm2 
        vpaddq  ymm1, ymm1, ymm3 
        vpaddq  ymm1, ymm1, ymm4 
        vpaddq  ymm1, ymm1, ymm5 
        vpaddq  ymm1, ymm1, ymm6 
        vpaddq  ymm1, ymm1, ymm7 

        vmovdqu [r8], ymm1     ; Store in y vector
        add r8, 32
        add rdx, 32             ; Move to the next aligned address (4 elements * 8 bytes)
        loop stencil

            
    end:
    ; Restore the stack and registers
    add rsp, 16
    pop rbp
    pop rdi
    pop rsi

    ret