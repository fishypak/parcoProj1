section .text
bits 64
default rel
global ymm1d

ymm1d:
    push rsi
    push rdi
    push rbp
    mov rbp, rsp
    sub rsp, 16  

    sub rsp,0x20
	vmovdqu [rsp],ymm6
	sub rsp,0x20
	vmovdqu [rsp],ymm7

    ; Number of times to loop
    mov rsi, rcx
    shr rcx, 2               ; div by 4 
    ;sub rcx, 1              
    jz end

    ; Perform stencil
    stencil:
        cmp rcx, 0
        jle end

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
        dec rcx
        jmp stencil

            
    end:
    ; Restore the stack and registers
    vmovdqu ymm7,[rsp]
	add rsp,0x20
	vmovdqu ymm6,[rsp]
	add rsp,0x20
    add rsp, 16
    pop rbp
    pop rdi
    pop rsi

    ret