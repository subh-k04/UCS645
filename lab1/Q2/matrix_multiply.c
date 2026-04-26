#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define N 1000

int A[N][N];
int B[N][N];
int C[N][N];

void initialize() {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            A[i][j] = i + j;
            B[i][j] = i * j;
        }
    }
}

int main() {
    initialize();

    double start_time, end_time;
    double seq_time;

    start_time = omp_get_wtime();
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            int sum = 0;
            for (int k = 0; k < N; k++) {
                sum += A[i][k] * B[k][j];
            }
            C[i][j] = sum;
        }
    }
    end_time = omp_get_wtime();
    seq_time = end_time - start_time;

    printf("Version: Sequential, Threads: 1, Time: %f, Speedup: 1.00\n", seq_time);

    int thread_counts[] = {2, 4, 6, 8};
    int num_counts = 4;

    for (int t = 0; t < num_counts; t++) {
        int threads = thread_counts[t];
        omp_set_num_threads(threads);

        start_time = omp_get_wtime();
        #pragma omp parallel for
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                int sum = 0;
                for (int k = 0; k < N; k++) {
                    sum += A[i][k] * B[k][j];
                }
                C[i][j] = sum;
            }
        }
        end_time = omp_get_wtime();
        double par_time = end_time - start_time;
        printf("Version: 1D, Threads: %d, Time: %f, Speedup: %f\n", threads, par_time, seq_time / par_time);
    }

    for (int t = 0; t < num_counts; t++) {
        int threads = thread_counts[t];
        omp_set_num_threads(threads);

        start_time = omp_get_wtime();
        #pragma omp parallel for collapse(2)
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                int sum = 0;
                for (int k = 0; k < N; k++) {
                    sum += A[i][k] * B[k][j];
                }
                C[i][j] = sum;
            }
        }
        end_time = omp_get_wtime();
        double par_time = end_time - start_time;
        printf("Version: 2D, Threads: %d, Time: %f, Speedup: %f\n", threads, par_time, seq_time / par_time);
    }

    return 0;
}
