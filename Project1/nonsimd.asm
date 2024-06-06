section .text

bits 64
default rel

global asm1d
asm1d:
    sub RCX, 6    ;ignore edges/halos

    block:

        xor RAX, RAX    ;clear accumulator

        ; -- main sum operation --
        ;unroll add 7x
        mov RAX, qword [rdx]
        add RAX, qword [rdx+8]
        add RAX, qword [rdx+16]
        add RAX, qword [rdx+24]
        add RAX, qword [rdx+32]
        add RAX, qword [rdx+40]
        add RAX, qword [rdx+48]

        mov [r8], RAX    ;store result in output

        ;move to next element
        ADD R8, 8        ;shift 1 input forward
        ADD RDX, 8        ;move to next output

    LOOP block

    ret