#include <stdio.h>

int main() {
    cudaDeviceProp prop;
    cudaGetDeviceProperties(&prop, 0);

    printf("GPU Name: %s\n", prop.name);
    printf("Compute Capability: %d.%d\n", prop.major, prop.minor);
    printf("Total Global Memory (MB): %.2f\n", prop.totalGlobalMem / (1024.0 * 1024.0));
    printf("Shared Memory per Block (KB): %.2f\n", prop.sharedMemPerBlock / 1024.0);
    printf("Constant Memory (KB): %.2f\n", prop.totalConstMem / 1024.0);
    printf("Warp Size: %d\n", prop.warpSize);
    printf("Max Threads per Block: %d\n", prop.maxThreadsPerBlock);
    printf("Max Block Dimensions: x=%d, y=%d, z=%d\n", prop.maxThreadsDim[0], prop.maxThreadsDim[1], prop.maxThreadsDim[2]);
    printf("Max Grid Dimensions: x=%d, y=%d, z=%d\n", prop.maxGridSize[0], prop.maxGridSize[1], prop.maxGridSize[2]);
    printf("Number of Multiprocessors: %d\n", prop.multiProcessorCount);
    
    int doubleSupport = (prop.major > 1) || (prop.major == 1 && prop.minor >= 3);
    printf("Double Precision Support: %s\n", doubleSupport ? "Yes" : "No");

    return 0;
}
