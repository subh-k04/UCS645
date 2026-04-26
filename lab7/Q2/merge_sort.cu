#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

void merge(int *arr, int l, int m, int r) {
    int n1 = m - l + 1;
    int n2 = r - m;
    int *L = (int*)malloc(n1 * sizeof(int));
    int *R = (int*)malloc(n2 * sizeof(int));
    for (int i = 0; i < n1; i++) L[i] = arr[l + i];
    for (int j = 0; j < n2; j++) R[j] = arr[m + 1 + j];
    
    int i = 0, j = 0, k = l;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) arr[k++] = L[i++];
        else arr[k++] = R[j++];
    }
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
    free(L); free(R);
}

void cpu_merge_sort(int *arr, int l, int r) {
    if (l < r) {
        int m = l + (r - l) / 2;
        cpu_merge_sort(arr, l, m);
        cpu_merge_sort(arr, m + 1, r);
        merge(arr, l, m, r);
    }
}

// Global memory merge sort phase for large N
__global__ void gpu_merge_phase(int *d_arr, int *d_temp, int n, int width) {
    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    long long left = 2LL * width * tid;
    
    if (left < n) {
        long long mid = left + width;
        long long right = left + 2LL * width;
        if (mid > n) mid = n;
        if (right > n) right = n;
        
        long long i = left, j = mid, k = left;
        while (i < mid && j < right) {
            if (d_arr[i] <= d_arr[j]) d_temp[k++] = d_arr[i++];
            else d_temp[k++] = d_arr[j++];
        }
        while (i < mid) d_temp[k++] = d_arr[i++];
        while (j < right) d_temp[k++] = d_arr[j++];
        
        for (i = left; i < right; i++) {
            d_arr[i] = d_temp[i];
        }
    }
}

int main() {
    int N = 1000;
    int *h_arr_cpu = (int*)malloc(N * sizeof(int));
    int *h_arr_gpu = (int*)malloc(N * sizeof(int));
    for (int i = 0; i < N; i++) {
        int v = rand() % 10000;
        h_arr_cpu[i] = v;
        h_arr_gpu[i] = v;
    }
    
    // Part A: CPU Sort
    LARGE_INTEGER freq, start_cpu, end_cpu;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&start_cpu);
    
    cpu_merge_sort(h_arr_cpu, 0, N - 1);
    
    QueryPerformanceCounter(&end_cpu);
    double cpu_time_ms = (double)(end_cpu.QuadPart - start_cpu.QuadPart) * 1000.0 / freq.QuadPart;
    
    // Part B: GPU Sort
    int *d_arr, *d_temp;
    cudaMalloc(&d_arr, N * sizeof(int));
    cudaMalloc(&d_temp, N * sizeof(int));
    cudaMemcpy(d_arr, h_arr_gpu, N * sizeof(int), cudaMemcpyHostToDevice);
    
    cudaEvent_t start_gpu, stop_gpu;
    cudaEventCreate(&start_gpu); cudaEventCreate(&stop_gpu);
    
    cudaEventRecord(start_gpu);
    
    // Launch bottom-up merge phases
    for (int width = 1; width < N; width *= 2) {
        int threads_needed = (N + 2 * width - 1) / (2 * width);
        int blocks = (threads_needed + 255) / 256;
        gpu_merge_phase<<<blocks, 256>>>(d_arr, d_temp, N, width);
    }
    
    cudaEventRecord(stop_gpu);
    cudaMemcpy(h_arr_gpu, d_arr, N * sizeof(int), cudaMemcpyDeviceToHost);
    cudaEventSynchronize(stop_gpu);
    
    float gpu_time = 0;
    cudaEventElapsedTime(&gpu_time, start_gpu, stop_gpu);
    
    // Verification
    int valid = 1;
    for (int i = 0; i < N - 1; i++) {
        if (h_arr_gpu[i] > h_arr_gpu[i+1]) valid = 0;
    }
    for (int i = 0; i < N; i++) {
        if (h_arr_cpu[i] != h_arr_gpu[i]) valid = 0;
    }
    
    // Part C: Comparision
    printf("Array size: %d\n", N);
    printf("CPU Merge Sort Time (QueryPerformanceCounter): %f ms\n", cpu_time_ms);
    printf("GPU Merge Sort Time (cudaEvent): %f ms\n", gpu_time);
    printf("Speedup (CPU/GPU): %f\n", cpu_time_ms / gpu_time);
    printf("GPU Validation: %s\n", valid ? "SUCCESS" : "FAILED");
    
    cudaFree(d_arr); cudaFree(d_temp);
    cudaEventDestroy(start_gpu); cudaEventDestroy(stop_gpu);
    free(h_arr_cpu); free(h_arr_gpu);
    return 0;
}
