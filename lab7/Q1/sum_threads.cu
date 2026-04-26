#include <stdio.h>
#include <stdlib.h>

__global__ void sum_kernel(int *input, long long *output, int n) {
    int tid = threadIdx.x;
    
    if (tid == 0) {
        long long sum = 0;
        for (int i = 0; i < n; i++) {
            sum += input[i];
        }
        output[0] = sum;
    } else if (tid == 1) {
        long long n_val = n;
        output[1] = n_val * (n_val + 1) / 2;
    } else {
        if (tid == 2 || tid == 1023) {
            printf("Thread %d ranking in, doing nothing.\n", tid);
        }
    }
}

int main() {
    int N = 1024;
    int *h_input = (int*)malloc(N * sizeof(int));
    long long *h_output = (long long*)malloc(2 * sizeof(long long));
    
    for (int i = 0; i < N; i++) {
        h_input[i] = i + 1;
    }
    
    int *d_input;
    long long *d_output;
    cudaMalloc(&d_input, N * sizeof(int));
    cudaMalloc(&d_output, 2 * sizeof(long long));
    
    cudaMemcpy(d_input, h_input, N * sizeof(int), cudaMemcpyHostToDevice);
    
    sum_kernel<<<1, 1024>>>(d_input, d_output, N);
    
    cudaMemcpy(h_output, d_output, 2 * sizeof(long long), cudaMemcpyDeviceToHost);
    cudaDeviceSynchronize();
    
    printf("Expected: 524800\n");
    printf("Iterative sum (Thread 0): %lld\n", h_output[0]);
    printf("Formula sum (Thread 1): %lld\n", h_output[1]);
    
    if (h_output[0] == h_output[1] && h_output[0] == 524800) {
        printf("Verification: MATCH\n");
    } else {
        printf("Verification: FAILED\n");
    }
    
    cudaFree(d_input);
    cudaFree(d_output);
    free(h_input);
    free(h_output);
    
    return 0;
}
