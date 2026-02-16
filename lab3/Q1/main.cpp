#include <iostream>
#include <vector>
#include <chrono>
#include <cstdlib>
#include <omp.h>

using namespace std;

// Function declarations
void correlate_sequential(int ny, int nx, const float* data, float* result);
void correlate_parallel(int ny, int nx, const float* data, float* result);
void correlate_optimized(int ny, int nx, const float* data, float* result);

int main(int argc, char* argv[])
{
    if (argc < 5)
    {
        printf("Usage: %s <ny> <nx> <threads> <mode>\n", argv[0]);
        printf("Mode: 0 = Sequential, 1 = Parallel, 2 = Optimized\n");
        return 1;
    }

    int ny = atoi(argv[1]);
    int nx = atoi(argv[2]);
    int threads = atoi(argv[3]);
    int mode = atoi(argv[4]);

    omp_set_num_threads(threads);

    printf("========================================\n");
    printf("Correlation Matrix Computation\n");
    printf("Matrix size : %d x %d\n", ny, nx);
    printf("Threads     : %d\n", threads);

    if (mode == 0)
        printf("Mode        : Sequential\n");
    else if (mode == 1)
        printf("Mode        : Parallel (OpenMP)\n");
    else
        printf("Mode        : Optimized (SIMD + OpenMP)\n");

    printf("========================================\n");

    // Allocate memory
    vector<float> data(ny * nx);
    vector<float> result(ny * ny, 0.0f);

    // Initialize random data (important for valid benchmark)
    srand(42);
    for (int i = 0; i < ny * nx; i++)
    {
        data[i] = (float)rand() / RAND_MAX;
    }

    // Warm-up run
    if (mode == 0)
        correlate_sequential(ny, nx, data.data(), result.data());
    else if (mode == 1)
        correlate_parallel(ny, nx, data.data(), result.data());
    else
        correlate_optimized(ny, nx, data.data(), result.data());

    // Start timing
    auto start = chrono::high_resolution_clock::now();

    if (mode == 0)
        correlate_sequential(ny, nx, data.data(), result.data());
    else if (mode == 1)
        correlate_parallel(ny, nx, data.data(), result.data());
    else
        correlate_optimized(ny, nx, data.data(), result.data());

    auto end = chrono::high_resolution_clock::now();

    chrono::duration<double> elapsed = end - start;

    printf("Execution Time: %.6f seconds\n", elapsed.count());
    printf("Sample result value: %.6f\n", result[ny - 1]);

    printf("========================================\n");

    return 0;
}
