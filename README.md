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
The cstencil function performs a stencil computation on an input array x of long long int values, outputting the result to array y. The function processes elements from x[3] to x[n-4], calculating the sum of seven contiguous elements centered on each x[i] from x[i-3] to x[i+3], and stores this sum in y[i-3]. This ensures that output starts from the beginning of the y array and avoids out-of-bound accesses.

### Screenshots

**C Output at 2 ^ 20 Debug Mode**

![image](https://github.com/fishypak/parcoProj1/assets/98962170/c852a56c-97b7-460e-99fe-b19b3fc38b89)

**C Output at 2 ^ 26 Debug Mode**

![image](https://github.com/fishypak/parcoProj1/assets/98962170/ba88e9f7-d08d-40a6-b07b-3352660da508)

**C Output at 2 ^ 30 Debug Mode**

![image](https://github.com/fishypak/parcoProj1/assets/98962170/9c525019-bb29-4a14-8f46-0ab3c3535fb6)

**C Output at 2 ^ 20 Release Mode**

![image](https://github.com/fishypak/parcoProj1/assets/98962170/8a71fa72-3074-4b43-b9b6-049387f93fa1)

**C Output at 2 ^ 26 Release Mode**

![image](https://github.com/fishypak/parcoProj1/assets/98962170/1bc68f86-59ae-4381-bdfa-6413253e7003)

**C Output at 2 ^ 30 Release Mode**

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

The assembly code asm1d implements a non-SIMD stencil computation for an array. The code adjusts the loop counter to skip processing the first and last three elements of the array for boundary safety. Inside the loop, it zeros out the accumulator register rax, loads and sums seven consecutive 64-bit integers from the input array x (pointed to by rdx), and stores the accumulated result into the output array y (pointed to by r8). After storing the result, the pointers r8 and rdx are incremented to the next element, and the loop continues until all valid elements have been processed.

### Screenshots

**x86-64 Output at 2 ^ 20 Debug Mode**

![image](https://github.com/fishypak/parcoProj1/assets/98962170/227ef943-04ac-4d62-badb-543bf4d88450)

**x86-64 Output at 2 ^ 26 Debug Mode**

![image](https://github.com/fishypak/parcoProj1/assets/98962170/3afc17b6-bc3b-4e70-bd8b-897079de3f89)

**x86-64 Output at 2 ^ 30 Debug Mode**

![image](https://github.com/fishypak/parcoProj1/assets/98962170/38d786e1-6b30-4911-aa12-58390d9cca4f)

**x86-64 Output at 2 ^ 20 Release Mode**

![image](https://github.com/fishypak/parcoProj1/assets/98962170/4fe02eb8-110f-4e46-9dcd-84c493c8ec10)

**x86-64 Output at 2 ^ 26 Release Mode**

![image](https://github.com/fishypak/parcoProj1/assets/98962170/4735256d-a53a-4891-bee3-9b7aedc4986a)

**x86-64 Output at 2 ^ 30 Release Mode**

![image](https://github.com/fishypak/parcoProj1/assets/98962170/445876ae-45a2-4dea-a5cd-7ecff8d8fb31)

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

The xmm1D assembly function efficiently implements a SIMD-based stencil operation using XMM registers to accelerate processing of 64-bit integers. The function sets up by saving essential registers and allocating stack space, then it initializes counters to manage array access within bounds. Within the main loop, it loads consecutive elements from the input array into XMM registers (xmm0 to xmm6) to handle seven adjacent values at once. Using the paddq instruction, these values are summed in a packed format, exploiting SIMD capabilities to perform multiple additions in parallel, thus significantly enhancing performance. The summed result is then stored back into the output array using movdqu, and the loop continues until all applicable elements are processed.

### Screenshots

**XMM Output at 2 ^ 20 Debug Mode**

![image](https://github.com/fishypak/parcoProj1/assets/98962170/8d147aa6-76ca-4bd2-a3d5-50d1b3bba541)

**XMM Output at 2 ^ 26 Debug Mode**

![image](https://github.com/fishypak/parcoProj1/assets/98962170/438971d1-6f14-420f-85c9-5d207b2ae36f)

**XMM Output at 2 ^ 30 Debug Mode**

![image](https://github.com/fishypak/parcoProj1/assets/98962170/86b58bcb-e8f6-4d40-8480-22a024b00862)

**XMM Output at 2 ^ 20 Release Mode**

![image](https://github.com/fishypak/parcoProj1/assets/98962170/65dd2830-5b1a-4417-927b-54c4a7669e03)

**XMM Output at 2 ^ 26 Release Mode**

![image](https://github.com/fishypak/parcoProj1/assets/98962170/b6557d62-b92b-4e3c-be15-bcb09c3b3cbd)

**XMM Output at 2 ^ 30 Release Mode**

![image](https://github.com/fishypak/parcoProj1/assets/98962170/37bcf1a3-7820-440c-a6bd-d35846514f73)

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

The ymm1d assembly function uses YMM registers for SIMD (Single Instruction, Multiple Data) operations to accelerate the processing of a stencil computation over 64-bit integers in an array. The function sets up by pushing essential registers onto the stack for later restoration and adjusts the stack pointer for local usage. It divides the total number of elements by 4 (using shr rcx, 2), optimizing the loop to process four elements simultaneously, reflecting the 256-bit width of YMM registers that handle four 64-bit integers at once. Within the loop, it loads seven consecutive data blocks into YMM registers (ymm1 to ymm7), sums them using vpaddq, and stores the result back into the output array. The pointers are adjusted by 32 bytes (4 elements of 8 bytes each) for the next set of inputs and outputs. The loop decrements and continues until all elements are processed, making effective use of the wider YMM registers to enhance computational throughput and efficiency for large data sets.

### Screenshots

**YMM Output at 2 ^ 20 Debug Mode**

![image](https://github.com/fishypak/parcoProj1/assets/98962170/165a6be3-290b-4b8c-9e8d-4e136b1e615a)

**YMM Output at 2 ^ 26 Debug Mode**

![image](https://github.com/fishypak/parcoProj1/assets/98962170/741637a8-7a97-437d-9706-35e76a67a9e0)

**YMM Output at 2 ^ 30 Debug Mode**

![image](https://github.com/fishypak/parcoProj1/assets/98962170/b117686c-4584-40c0-bc46-5f51e300fd4e)

**YMM Output at 2 ^ 20 Release Mode**

![image](https://github.com/fishypak/parcoProj1/assets/98962170/227bb2a1-fc55-4367-96e5-c46958337483)

**YMM Output at 2 ^ 26 Release Mode**

![image](https://github.com/fishypak/parcoProj1/assets/98962170/05f63e6a-b4d3-4fdb-97ad-310d6bb9681d)

**YMM Output at 2 ^ 30 Release Mode**

![image](https://github.com/fishypak/parcoProj1/assets/98962170/e2ad228d-fe4b-435f-a242-0b4eb1335805)

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

In Debug mode, we observe that the execution times decrease significantly with the more advanced implementations of the stencil computation, particularly when leveraging SIMD capabilities:

- C Implementation: The slowest across all precisions, which is expected due to the lack of parallel processing capabilities.
- x86-64 ASM: Shows a moderate performance improvement over the C implementation due to the more direct control over hardware, but still processes data sequentially.
- x86 SIMD AVX2 using XMM: Offers a considerable performance boost by processing multiple data points simultaneously. For instance, at precision 2^30, it runs approximately 1.9 times faster than the x86-64 ASM implementation.
- x86 SIMD AVX2 using YMM: The fastest implementation due to its ability to handle even more data per operation than XMM. At precision 2^30, YMM runs around 1.1 times faster than XMM and about 2.1 times faster than x86-64 ASM.

The increasing efficiency from C to YMM in Debug mode is attributed to better utilization of the CPU's ability to perform parallel operations, reducing the number of instructions and memory accesses needed to complete the same amount of work.

In Release mode, the C implementation outperforms the x86-64 ASM and the x86 SIMD AVX2 using XMM implementations at certain precisions. This is due to the fact that in release mode, the compiler optimizes the C code, allowing it to match or exceed the performance of manually written assembly.

## Discussion

Discuss the problems encountered and solutions made, unique methodology used, AHA moments, etc. 
