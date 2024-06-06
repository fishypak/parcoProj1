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