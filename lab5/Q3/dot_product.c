#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define TOTAL_ELEMENTS 500000000

int main(int argc, char** argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    double time_start = MPI_Wtime();

    double multiplier;
    if (rank == 0) {
        multiplier = 2.0;
    }
    MPI_Bcast(&multiplier, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    long long chunk_size = TOTAL_ELEMENTS / size;
    double *A = (double*)malloc(chunk_size * sizeof(double));
    double *B = (double*)malloc(chunk_size * sizeof(double));

    for (long long i = 0; i < chunk_size; i++) {
        A[i] = 1.0;
        B[i] = 2.0 * multiplier;
    }

    double local_dot = 0.0;
    for (long long i = 0; i < chunk_size; i++) {
        local_dot += A[i] * B[i];
    }

    double global_dot = 0.0;
    MPI_Reduce(&local_dot, &global_dot, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    double time_end = MPI_Wtime();

    if (rank == 0) {
        printf("Total time: %f sec\n", time_end - time_start);
        printf("Final dot product: %f\n", global_dot);
    }

    free(A);
    free(B);
    MPI_Finalize();
    return 0;
}
