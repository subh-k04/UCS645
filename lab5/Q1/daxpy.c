#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define N 65536
#define A 2.5

int main(int argc, char** argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    double *X = NULL, *Y = NULL;
    if (rank == 0) {
        X = (double*)malloc(N * sizeof(double));
        Y = (double*)malloc(N * sizeof(double));
        for (int i = 0; i < N; i++) {
            X[i] = i * 1.0;
            Y[i] = i * 2.0;
        }
    }

    double seq_time = 0.0, par_time = 0.0;

    // Sequential timing on process 0
    if (rank == 0) {
        double *X_seq = (double*)malloc(N * sizeof(double));
        for (int i = 0; i < N; i++) X_seq[i] = X[i];

        MPI_Barrier(MPI_COMM_WORLD);
        double t_start = MPI_Wtime();
        for (int i = 0; i < N; i++) {
            X_seq[i] = A * X_seq[i] + Y[i];
        }
        double t_end = MPI_Wtime();
        seq_time = t_end - t_start;
        free(X_seq);
    } else {
        MPI_Barrier(MPI_COMM_WORLD);
    }

    // Parallel execution
    int chunk_size = N / size;
    double *local_X = (double*)malloc(chunk_size * sizeof(double));
    double *local_Y = (double*)malloc(chunk_size * sizeof(double));

    MPI_Scatter(X, chunk_size, MPI_DOUBLE, local_X, chunk_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatter(Y, chunk_size, MPI_DOUBLE, local_Y, chunk_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    double par_start = MPI_Wtime();
    for (int i = 0; i < chunk_size; i++) {
        local_X[i] = A * local_X[i] + local_Y[i];
    }
    double par_end = MPI_Wtime();

    double local_par_time = par_end - par_start;
    MPI_Reduce(&local_par_time, &par_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    MPI_Gather(local_X, chunk_size, MPI_DOUBLE, X, chunk_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("Sequential time: %f sec\n", seq_time);
        printf("Parallel time: %f sec\n", par_time);
        printf("Speedup: %f\n", seq_time / par_time);
        free(X);
        free(Y);
    }
    
    free(local_X);
    free(local_Y);
    MPI_Finalize();
    return 0;
}
