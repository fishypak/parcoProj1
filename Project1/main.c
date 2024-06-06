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
	size_t n = 1 << 20;
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
	double timeTakenC, totalTC, avgTC;
	double timeTakenN, totalTN, avgTN;
	double timeTakenX, totalTX, avgTX;
	double timeTakenY, totalTY, avgTY;
	double timeTakenArr[30];
	double totalTime;

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
	
	for (int i = 0; i < 30; i++) {
		timeTakenArr[i] = 0;
	}

	//---------------- C STENCIL --------------------/*
	printf("Total Elements: %zd\n", n);
	totalTC = 0.0;
	totalTime = 0;
	for (int i = 0; i < 30; i++) {
		QueryPerformanceCounter(&start);
		cstencil(n, x, y);
		QueryPerformanceCounter(&end);
		timeTakenC = (double)(end.QuadPart - start.QuadPart) * 1000 / freq.QuadPart;
		totalTC = totalTC + timeTakenC;
		timeTakenArr[i] = timeTakenC;
	}

	for (int i = 0; i < 30; i++) {
		totalTime += timeTakenArr[i];
	}

	avgTC = totalTime / 30.0;
	printf("Total Time in C for 30 times: %f ms\nAverage Time in C: %f ms\n", totalTC, avgTC);
	printf("new total time: %f ms\n", totalTime);
	printf("First 10 elements in C: ");
	for (int i = 0; i < 10; i++) {
		printf("%lld ", y[i]);
	}
	printf("\nLast 10 elements in C: ");
	for (int i = n - 16; i < n - 6; i++) {
		printf("%lld ", y[i]);
	}

	for (int i = 0; i < 30; i++) {
		timeTakenArr[i] = 0;
	}
	
	//------------------ ASM STENCIL -------------------
	totalTN = 0.0;
	totalTime = 0;
	for (int i = 0; i < 30; i++) {
		QueryPerformanceCounter(&start);
		asm1d(n, x, yasm);
		QueryPerformanceCounter(&end);
		timeTakenN = (double)(end.QuadPart - start.QuadPart) * 1000 / freq.QuadPart;
		totalTN = totalTN + timeTakenN;
		timeTakenArr[i] = timeTakenN;
	}

	for (int i = 0; i < 30; i++) {
		totalTime += timeTakenArr[i];
	}

	avgTN = totalTime / 30.0;
	printf("\n\n\nTotal Time in Non-SIMD for 30 times: %f ms\nAverage Time in Non-SIMD: %f ms\n", totalTN, avgTN);
	printf("new total time: %f ms\n", totalTime);
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
	printf("Total Errors: %d\n\n\n", err);

	for (int i = 0; i < 30; i++) {
		timeTakenArr[i] = 0;
	}
	
	//------------------ XMM STENCIL ------------------
	totalTX = 0.0;
	totalTime = 0;
	for (int i = 0; i < 30; i++) {
		QueryPerformanceCounter(&start);
		xmm1D(n, x, xmmy);
		QueryPerformanceCounter(&end);
		timeTakenX = (double)(end.QuadPart - start.QuadPart) * 1000 / freq.QuadPart;
		totalTX = totalTX + timeTakenX;
		timeTakenArr[i] = timeTakenX;
	}

	for (int i = 0; i < 30; i++) {
		totalTime += timeTakenArr[i];
	}

	avgTX = totalTime / 30.0;
	printf("\nTotal Time in XMM for 30 times: %f ms\nAverage Time in XMM: %f ms\n", totalTime, avgTX);
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
	printf("Total Errors: %d\n\n\n", err);

	for (int i = 0; i < 30; i++) {
		timeTakenArr[i] = 0;
	}
	
// -------------------- YMM STENCIL -------------------
	totalTY = 0.0;
	totalTime = 0;
	for (int i = 0; i < 30; i++) {
		QueryPerformanceCounter(&start);
		ymm1d(n, x, ymmy);
		QueryPerformanceCounter(&end);
		timeTakenY = (double)(end.QuadPart - start.QuadPart) * 1000 / freq.QuadPart;
		totalTY = totalTY + timeTakenY;
		timeTakenArr[i] = timeTakenY;
	}

	for (int i = 0; i < 30; i++) {
		totalTime += timeTakenArr[i];
	}

	avgTY = totalTime / 30.0;
	printf("\nTotal Time in YMM for 30 times: %f ms\nAverage Time in Y: %f ms\n", totalTime, avgTY);
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
	printf("Total Errors: %d\n", err);

	free(x);
	free(y);
	free(yasm);
	free(xmmy);
	free(ymmy);
	

	return 0;
}