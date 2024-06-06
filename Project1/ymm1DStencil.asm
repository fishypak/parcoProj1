section .text
bits 64
default rel
global ymm1DStencil

; RCX - ARRAY_SIZE
; RDX - Address of input vector x -> [rdx] is value of x 
; R8 - Address of output vector y -> [r8] is value of y

ymm1DStencil:
    ; Number of times to loop
    shr rcx, 3              ; YMM register (256-bit) / uint32_t (32-bit) == 4 elements processed at once
    sub rcx, 1              ; Maximum: n - 1 -> Last set of 8 elements will cause an out-of-bounds

    ; Starting index of 2nd param - Y vector
    add rdx, 12             ; 3rd index (4 bytes * 3)

    ; Perform stencil
    Lsum:
        ; Move values to registers
        vmovdqu ymm1, [rdx]     
        vmovdqu ymm2, [rdx + 4]  
        vmovdqu ymm3, [rdx + 8]  
        vmovdqu ymm4, [rdx + 12] 
        vmovdqu ymm5, [rdx + 16] 
        vmovdqu ymm6, [rdx + 20] 
        vmovdqu ymm7, [rdx + 24] 

        vpaddd  ymm1, ymm1, ymm2 
        vpaddd  ymm1, ymm1, ymm3 
        vpaddd  ymm1, ymm1, ymm4 
        vpaddd  ymm1, ymm1, ymm5 
        vpaddd  ymm1, ymm1, ymm6 
        vpaddd  ymm1, ymm1, ymm7 

        vmovdqu [rdx], ymm1     ; Store 4 results in Y vector
        add rdx, 16             ; Move to the next aligned address (4 elements * 4 bytes)
        loop Lsum

    ret