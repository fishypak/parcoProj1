#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <stdint.h>
#include <time.h>

extern void ymm1d(size_t ARRAY_SIZE, long long int* x, long long int* y);

extern void xmm1D(size_t n, long long int* x, long long int* y);

extern void asm1d(size_t n, long long int* x, long long int* y);

void cstencil(size_t n, long long int* x, long long int* y) {

	for (size_t i = 3; i < n - 3; i++) {
		y[i - 3] = x[i - 3] + x[i - 2] + x[i - 1] + x[i] + x[i + 1] + x[i + 2] + x[i + 3];
	}
}

int main() {
	srand(time(NULL));

	long long int* x, * y, * yasm, * xmmy, * ymmy;
	size_t n = 1 << 30;
	LARGE_INTEGER start, end, freq;
	const size_t ARRAY_BYTES = n * sizeof(long long int);
	const size_t ARRAY_BYTES_Y = (n - 6) * sizeof(long long int);
	int err;

	QueryPerformanceFrequency(&freq);
	x = (long long int*)malloc(ARRAY_BYTES);
	y = (long long int*)malloc(ARRAY_BYTES_Y);
	yasm = (long long int*)malloc(ARRAY_BYTES_Y);
	xmmy = (long long int*)malloc(ARRAY_BYTES_Y);
	ymmy = (long long int*)malloc(ARRAY_BYTES_Y);
	double timeTaken, totalT, avgT;

	//initialization
	for (int i = 0; i < n; i++) {
		x[i] = rand() % 16;
	}
	for (int i = 0; i < n - 6; i++) {
		y[i] = 0;
		yasm[i] = 0;
		xmmy[i] = 0;
		ymmy[i] = 0;
	}

	//---------------- C STENCIL --------------------/*
	printf("Total Elements: %zd\n", n);
	totalT = 0.0;
	for (int i = 0; i < 30; i++) {
		QueryPerformanceCounter(&start);
		cstencil(n, x, y);
		QueryPerformanceCounter(&end);
		timeTaken = (double)(end.QuadPart - start.QuadPart) * 1000 / freq.QuadPart;
		totalT = totalT + timeTaken;
	}
	avgT = totalT / 30.0;
	printf("Total Time in C for 30 times: %f ms\nAverage Time in C: %f ms\n", totalT, avgT);
	printf("First 10 elements in C: ");
	for (int i = 0; i < 10; i++) {
		printf("%lld ", y[i]);
	}
	printf("\nLast 10 elements in C: ");
	for (int i = n - 16; i < n - 6; i++) {
		printf("%lld ", y[i]);
	}
	/*
	//------------------ ASM STENCIL -------------------
	totalT = 0.0;
	for (int i = 0; i < 30; i++) {
		QueryPerformanceCounter(&start);
		asm1d(n, x, yasm);
		QueryPerformanceCounter(&end);
		timeTaken = (double)(end.QuadPart - start.QuadPart) * 1000 / freq.QuadPart;
		totalT = totalT + timeTaken;
	}
	avgT = totalT / 30.0;
	printf("\n\nTotal Time in Non-SIMD for 30 times: %f ms\nAverage Time in C: %f ms\n", totalT, avgT);
	printf("First 10 elements in Non-SIMD: ");
	for (int i = 0; i < 10; i++) {
		printf("%lld ", yasm[i]);
	}
	printf("\nLast 10 elements in Non-SIMD: ");
	for (int i = n - 16; i < n - 6; i++) {
		printf("%lld ", yasm[i]);
	}

	err = 0;
	for (size_t i = 0; i < n-6; i++) {
		if (y[i] != yasm[i])
			err += 1;
	}
	printf("Total Errors: %d", err);
	*/
	//------------------ XMM STENCIL ------------------
	totalT = 0.0;
	for (int i = 0; i < 30; i++) {
		QueryPerformanceCounter(&start);
		xmm1D(n, x, xmmy);
		QueryPerformanceCounter(&end);
		timeTaken = (double)(end.QuadPart - start.QuadPart) * 1000 / freq.QuadPart;
		totalT = totalT + timeTaken;
	}
	avgT = totalT / 30.0;
	printf("\n\nTotal Time in XMM for 30 times: %f ms\nAverage Time in C: %f ms\n", totalT, avgT);
	printf("First 10 elements in XMM: ");
	for (int i = 0; i < 10; i++) {
		printf("%lld ", xmmy[i]);
	}
	printf("\nLast 10 elements in XMM: ");
	for (int i = n - 16; i < n - 6; i++) {
		printf("%lld ", xmmy[i]);
	}
	err = 0;
	for (size_t i = 0; i < n-6; i++) {
		if (y[i] != xmmy[i])
			err += 1;
	}
	printf("Total Errors: %d", err);
// -------------------- YMM STENCIL -------------------
	totalT = 0.0;
	for (int i = 0; i < 30; i++) {
		QueryPerformanceCounter(&start);
		ymm1d(n, x, ymmy);
		QueryPerformanceCounter(&end);
		timeTaken = (double)(end.QuadPart - start.QuadPart) * 1000 / freq.QuadPart;
		totalT = totalT + timeTaken;
	}
	avgT = totalT / 30.0;
	printf("\n\nTotal Time in YMM for 30 times: %f ms\nAverage Time in Y: %f ms\n", totalT, avgT);
	printf("First 10 elements in YMM: ");
	for (int i = 0; i < 10; i++) {
		printf("%lld ", ymmy[i]);
	}
	printf("\nLast 10 elements in YMM: ");
	for (int i = n - 16; i < n - 6; i++) {
		printf("%lld ", ymmy[i]);
	}
	err = 0;
	for (size_t i = 0; i < n-6; i++) {
		if (y[i] != ymmy[i])
			err += 1;
	}
	printf("Total Errors: %d", err);
	
	free(x);
	free(y);
	free(xmmy);
	free(ymmy);
	return 0;
}