# CEPARCO 1-D Stencil SIMD Programming Project
in (1) C Program (2) x86-64 Assembly Language (3) x86 SIMD AVX2 assembly language - XMM (4) x86 SIMD AVX2 assembly language - YMM

Input: Memory location n (integer) contains the length of the vector; Vectors X and Y are both 32-bit integer.

Process: Y[i] = X[i - 3] + X[i - 2] + X[i - 1] + X[i] + X[i + 1] + X[i + 2] + X[i + 3]

Output: store result in vector Y. Also, display the result of first ten elements and last 10 elements of vector Y for all versions of kernels.

Example: X-> 1.2.3.4.5.6.7.8: output: Y-> 28.35

## Implementation

## C

### Code 

```
void cstencil(size_t n, long long int* x, long long int* y) {

	for (size_t i = 3; i < n - 3; i++) {
		y[i - 3] = x[i - 3] + x[i - 2] + x[i - 1] + x[i] + x[i + 1] + x[i + 2] + x[i + 3];
	}
}
```

### Screenshots

**2 ^ 20 Debug Mode**

![image](https://github.com/fishypak/parcoProj1/assets/98962170/c852a56c-97b7-460e-99fe-b19b3fc38b89)

**2 ^ 26 Debug Mode**

![image](https://github.com/fishypak/parcoProj1/assets/98962170/ba88e9f7-d08d-40a6-b07b-3352660da508)

**2 ^ 30 Debug Mode**

![image](https://github.com/fishypak/parcoProj1/assets/98962170/9c525019-bb29-4a14-8f46-0ab3c3535fb6)

**2 ^ 20 Release Mode**

![image](https://github.com/fishypak/parcoProj1/assets/98962170/8a71fa72-3074-4b43-b9b6-049387f93fa1)

**2 ^ 26 Release Mode**

![image](https://github.com/fishypak/parcoProj1/assets/98962170/1bc68f86-59ae-4381-bdfa-6413253e7003)

**2 ^ 30 Release Mode**

![image](https://github.com/fishypak/parcoProj1/assets/98962170/816390f9-2fda-407c-8483-a7cc67907fc8)


## x86-64 ASM

### Code 

```
section .text
bits 64
default rel

global asm1d
asm1d:
    sub rcx, 6    ; adjust the counter to ignore edge elements

process_elements:
    xor rax, rax 

    ; load and accumulate values from the input array
    mov rax, qword [rdx]       
    add rax, qword [rdx+8] 
    add rax, qword [rdx+16]
    add rax, qword [rdx+24] 
    add rax, qword [rdx+32]
    add rax, qword [rdx+40] 
    add rax, qword [rdx+48]

    mov [r8], rax               ; store the accumulated sum in the output array

    ; prepare for the next iteration
    add r8, 8
    add rdx, 8

    loop process_elements        

    ret
```

### Screenshots

## x86 SIMD AVX2 ASM using XMM

### Code 

```
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
    add rsp, 16
    pop rbp
    pop rdi
    pop rsi
    ret
```

### Screenshots

## x86 SIMD AVX2 ASM using YMM

### Code 

```
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
    add rsp, 16
    pop rbp
    pop rdi
    pop rsi

    ret
```

### Screenshots

## Table of Execution Time

Execution times for each implementation are measured in both DEBUG and RELEASE mode. The kernel is ran 30 times and the average runtime is calculated. A single run is also executed as a buffer to eliminate the first run performance outlier. Below are the tables of average runtime of each implementation in Debug and Release modes.

### Debug Mode

| Precision | C (ms)        | x86-64 ASM (ms) | x86 SIMD AVX2 ASM using XMM (ms) | x86 SIMD AVX2 ASM using YMM (ms) |
|-----------|---------------|-----------------|----------------------------------|----------------------------------|
| 2^20      | 1.930667      | 1.540277        | 0.920473                         | 0.423710                         |
| 2^26      | 128.821770    | 98.906903       | 66.722230                        | 60.782260                        |
| 2^30      | 3304.265307   | 2608.301087     | 1751.191687                      | 1561.732667                      |

### Release Mode

| Precision | C (ms)        | x86-64 ASM (ms) | x86 SIMD AVX2 ASM using XMM (ms) | x86 SIMD AVX2 ASM using YMM (ms) |
|-----------|---------------|-----------------|----------------------------------|----------------------------------|
| 2^20      | 0.414023      | 1.496637        | 0.657480                         | 0.369903                         |
| 2^26      | 63.689330     | 98.401087       | 65.950987                        | 60.936833                        |
| 2^30      | 2348.976547   | 2500.941850     | 1725.390667                      | 1489.474447                      |



## Performance Analysis 

## Discussion

Discuss the problems encountered and solutions made, unique methodology used, AHA moments, etc. 
