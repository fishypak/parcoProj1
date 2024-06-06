# CEPARCO 1-D Stencil SIMD Programming Project
in (1) C Program (2) x86-64 Assembly Language (3) x86 SIMD AVX2 assembly language - XMM (4) x86 SIMD AVX2 assembly language - YMM

Input: Memory location n (integer) contains the length of the vector; Vectors X and Y are both 32-bit integer.

Process: Y[i] = X[i - 3] + X[i - 2] + X[i - 1] + X[i] + X[i + 1] + X[i + 2] + X[i + 3]

Output: store result in vector Y. Also, display the result of first ten elements and last 10 elements of vector Y for all versions of kernels.

Example: X-> 1.2.3.4.5.6.7.8: output: Y-> 28.35

