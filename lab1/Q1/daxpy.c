#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define N 65536

int main() {
    double *X = (double *)malloc(N * sizeof(double));
    double *Y = (double *)malloc(N * sizeof(double));
    double a = 2.5;

    for (int i = 0; i < N; i++) {
        X[i] = i * 1.0;
        Y[i] = i * 2.0;
    }

    double start_time, end_time;
    double seq_time;

    start_time = omp_get_wtime();
    for (int i = 0; i < N; i++) {
        X[i] = a * X[i] + Y[i];
    }
    end_time = omp_get_wtime();
    seq_time = end_time - start_time;

    printf("Threads: 1, Time: %f, Speedup: 1.00\n", seq_time);

    int thread_counts[] = {2, 4, 6, 8, 12};
    int num_counts = 5;

    double max_speedup = 0.0;
    int best_threads = 1;

    for (int t = 0; t < num_counts; t++) {
        int threads = thread_counts[t];
        omp_set_num_threads(threads);

        for (int i = 0; i < N; i++) {
            X[i] = i * 1.0;
            Y[i] = i * 2.0;
        }

        start_time = omp_get_wtime();
        #pragma omp parallel for
        for (int i = 0; i < N; i++) {
            X[i] = a * X[i] + Y[i];
        }
        end_time = omp_get_wtime();
        
        double par_time = end_time - start_time;
        double speedup = seq_time / par_time;

        printf("Threads: %d, Time: %f, Speedup: %f\n", threads, par_time, speedup);

        if (speedup > max_speedup) {
            max_speedup = speedup;
            best_threads = threads;
        }
    }

    printf("Max speedup achieved with %d threads.\n", best_threads);

    free(X);
    free(Y);
    return 0;
}
