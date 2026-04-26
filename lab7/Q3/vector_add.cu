#include <stdio.h>
#include <stdlib.h>

#define N (1 << 20)

__device__ float A[N];
__device__ float B[N];
__device__ float C[N];

__global__ void vec_add() {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i < N) {
        C[i] = A[i] + B[i];
    }
}

int main() {
    float *h_A = (float*)malloc(N * sizeof(float));
    float *h_B = (float*)malloc(N * sizeof(float));
    float *h_C = (float*)malloc(N * sizeof(float));
    
    for (int i = 0; i < N; i++) {
        h_A[i] = 1.0f; 
        h_B[i] = 2.0f;
    }
    
    // 1.1 Copy to static device symbols
    cudaMemcpyToSymbol(A, h_A, N * sizeof(float));
    cudaMemcpyToSymbol(B, h_B, N * sizeof(float));
    
    // 1.2 Timing execution
    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);
    
    cudaEventRecord(start);
    vec_add<<<(N + 255)/256, 256>>>();
    cudaEventRecord(stop);
    cudaEventSynchronize(stop);
    
    cudaMemcpyFromSymbol(h_C, C, N * sizeof(float));
    
    float ms = 0;
    cudaEventElapsedTime(&ms, start, stop);
    float time_in_seconds = ms / 1000.0f;
    
    printf("1.2 - Kernel Execution Time: %f ms\n", ms);
    
    // Verify math visually
    printf("Sample result C[0] = %f (expected 3.0)\n", h_C[0]);
    
    // 1.3 Theoretical bandwidth for GTX 1650
    double memClockRate = 6001000.0; // KHz
    double busWidth = 128.0;         // bits
    
    // Formula calculation
    double theoBW = 2.0 * memClockRate * (busWidth / 8.0) / 1000000.0;
    printf("1.3 - Theoretical Bandwidth: %f GB/s\n", theoBW);
    
    // 1.4 Measured bandwidth
    double RBytes = N * sizeof(float) * 2; // Read A and B
    double WBytes = N * sizeof(float) * 1; // Write C
    double totalBytes = RBytes + WBytes;
    
    double measBW = (totalBytes / time_in_seconds) / 1000000000.0;
    
    printf("1.4 - Measured Bandwidth: %f GB/s\n", measBW);
    printf("1.4 - Bandwidth Efficiency: %f %%\n", (measBW / theoBW) * 100.0);
    
    free(h_A); 
    free(h_B); 
    free(h_C);
    
    return 0;
}
