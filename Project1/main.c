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

void resetTimeArr(double* arr) {
	for (int i = 0; i < 30; i++) {
		arr[i] = 0;
	}
}

double calcTotalTime(double* arr) {
	double totalTime = 0;
	for (int i = 0; i < 30; i++) {
		totalTime += arr[i];
	}

	return totalTime;
}

int main() {
	srand(time(NULL));

	long long int* x, * y, * yasm, * xmmy, * ymmy;
	size_t n = (1 << 26);
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
	double timeTaken, avgT;
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
	
	resetTimeArr(timeTakenArr);

	//---------------- C STENCIL --------------------/*
	printf("Total Elements: %zd\n", n);
	for (int i = 0; i < 30; i++) {
		QueryPerformanceCounter(&start);
		cstencil(n, x, y);
		QueryPerformanceCounter(&end);
		timeTaken = (double)(end.QuadPart - start.QuadPart) * 1000 / freq.QuadPart;
		timeTakenArr[i] = timeTaken;
	}

	totalTime = calcTotalTime(timeTakenArr);

	printf("\nTotal Time in C for 30 times: %f ms\n", totalTime);
	avgT = totalTime / 30.0;
	printf("Average Time in C: %f ms\n", avgT);
	printf("First 10 elements in C: ");
	for (int i = 0; i < 10; i++) {
		printf("%lld ", y[i]);
	}
	printf("\nLast 10 elements in C: ");
	for (int i = n - 16; i < n - 6; i++) {
		printf("%lld ", y[i]);
	}

	resetTimeArr(timeTakenArr);
	
	//------------------ ASM STENCIL -------------------
	for (int i = 0; i < 30; i++) {
		QueryPerformanceCounter(&start);
		asm1d(n, x, yasm);
		QueryPerformanceCounter(&end);
		timeTaken = (double)(end.QuadPart - start.QuadPart) * 1000 / freq.QuadPart;
		timeTakenArr[i] = timeTaken;
	}

	totalTime = calcTotalTime(timeTakenArr);

	printf("\n\n\nTotal Time in NON-SIMD for 30 times: %f ms\n", totalTime);
	avgT = totalTime / 30.0;
	printf("Average Time in NON-SIMD: %f ms\n", avgT);
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

	resetTimeArr(timeTakenArr);
	
	//------------------ XMM STENCIL ------------------
	for (int i = 0; i < 30; i++) {
		QueryPerformanceCounter(&start);
		xmm1D(n, x, xmmy);
		QueryPerformanceCounter(&end);
		timeTaken = (double)(end.QuadPart - start.QuadPart) * 1000 / freq.QuadPart;
		timeTakenArr[i] = timeTaken;
	}

	totalTime = calcTotalTime(timeTakenArr);

	printf("\nTotal Time in XMM for 30 times: %f ms\n", totalTime);
	avgT = totalTime / 30.0;
	printf("Average Time in XMM: %f ms\n", avgT);
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

	resetTimeArr(timeTakenArr);
	
// -------------------- YMM STENCIL -------------------
	for (int i = 0; i < 30; i++) {
		QueryPerformanceCounter(&start);
		ymm1d(n, x, ymmy);
		QueryPerformanceCounter(&end);
		timeTaken = (double)(end.QuadPart - start.QuadPart) * 1000 / freq.QuadPart;
		timeTakenArr[i] = timeTaken;
	}

	totalTime = calcTotalTime(timeTakenArr);

	printf("\nTotal Time in YMM for 30 times: %f ms\n", totalTime);
	avgT = totalTime / 30.0;
	printf("Average Time in YMM: %f ms\n", avgT);
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
	

	return 0;
}