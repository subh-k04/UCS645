#include <stdio.h>
#include <stdlib.h>

#define N 1024
#define BLOCK_SIZE 16

__global__ void matrix_add(int *a, int *b, int *c, int n) {
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;

    if (row < n && col < n) {
        c[row * n + col] = a[row * n + col] + b[row * n + col];
    }
}

int main() {
    int size = N * N * sizeof(int);
    int *h_a = (int*)malloc(size);
    int *h_b = (int*)malloc(size);
    int *h_c = (int*)malloc(size);

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            h_a[i * N + j] = i + j;
            h_b[i * N + j] = i * j;
        }
    }

    int *d_a, *d_b, *d_c;
    cudaMalloc(&d_a, size);
    cudaMalloc(&d_b, size);
    cudaMalloc(&d_c, size);

    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);

    cudaMemcpy(d_a, h_a, size, cudaMemcpyHostToDevice);
    cudaMemcpy(d_b, h_b, size, cudaMemcpyHostToDevice);

    dim3 dimBlock(BLOCK_SIZE, BLOCK_SIZE);
    dim3 dimGrid((N + BLOCK_SIZE - 1) / BLOCK_SIZE, (N + BLOCK_SIZE - 1) / BLOCK_SIZE);

    cudaEventRecord(start);
    matrix_add<<<dimGrid, dimBlock>>>(d_a, d_b, d_c, N);
    cudaEventRecord(stop);

    cudaMemcpy(h_c, d_c, size, cudaMemcpyDeviceToHost);

    cudaEventSynchronize(stop);
    float milliseconds = 0;
    cudaEventElapsedTime(&milliseconds, start, stop);

    printf("Matrix Size: %dx%d\n", N, N);
    printf("Number of floating point operations (additions): %d\n", N * N);
    printf("Number of global memory reads: %d\n", 2 * N * N);
    printf("Number of global memory writes: %d\n", N * N);
    printf("Sample C[0][0]: %d\n", h_c[0]);
    printf("Sample C[512][512]: %d\n", h_c[512 * N + 512]);
    printf("Sample C[1023][1023]: %d\n", h_c[1023 * N + 1023]);
    printf("Time Taken (ms): %f\n", milliseconds);

    cudaFree(d_a);
    cudaFree(d_b);
    cudaFree(d_c);
    cudaEventDestroy(start);
    cudaEventDestroy(stop);
    free(h_a);
    free(h_b);
    free(h_c);
    
    return 0;
}
