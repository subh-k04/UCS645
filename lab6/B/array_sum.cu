#include <stdio.h>
#include <stdlib.h>

#define N 1000000
#define THREADS_PER_BLOCK 256

__global__ void sum_reduction(float *d_in, float *d_out, int n) {
    __shared__ float sdata[THREADS_PER_BLOCK];
    int tid = threadIdx.x;
    int i = blockIdx.x * blockDim.x + threadIdx.x;

    sdata[tid] = (i < n) ? d_in[i] : 0.0f;
    __syncthreads();

    for (int s = blockDim.x / 2; s > 0; s >>= 1) {
        if (tid < s) {
            sdata[tid] += sdata[tid + s];
        }
        __syncthreads();
    }

    if (tid == 0) {
        d_out[blockIdx.x] = sdata[0];
    }
}

int main() {
    float *h_in = (float*)malloc(N * sizeof(float));
    for (int i = 0; i < N; i++) h_in[i] = 1.0f;

    int num_blocks = (N + THREADS_PER_BLOCK - 1) / THREADS_PER_BLOCK;
    float *h_out = (float*)malloc(num_blocks * sizeof(float));

    float *d_in, *d_out;
    cudaMalloc(&d_in, N * sizeof(float));
    cudaMalloc(&d_out, num_blocks * sizeof(float));

    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);

    cudaMemcpy(d_in, h_in, N * sizeof(float), cudaMemcpyHostToDevice);

    cudaEventRecord(start);
    sum_reduction<<<num_blocks, THREADS_PER_BLOCK>>>(d_in, d_out, N);
    cudaEventRecord(stop);

    cudaMemcpy(h_out, d_out, num_blocks * sizeof(float), cudaMemcpyDeviceToHost);

    cudaEventSynchronize(stop);
    float milliseconds = 0;
    cudaEventElapsedTime(&milliseconds, start, stop);

    float final_sum = 0.0f;
    for (int i = 0; i < num_blocks; i++) {
        final_sum += h_out[i];
    }

    printf("Array Size: %d\n", N);
    printf("Expected Sum: %.1f\n", 1000000.0);
    printf("Computed Sum: %.1f\n", final_sum);
    printf("Time Taken (ms): %f\n", milliseconds);

    cudaFree(d_in);
    cudaFree(d_out);
    cudaEventDestroy(start);
    cudaEventDestroy(stop);
    free(h_in);
    free(h_out);
    return 0;
}
