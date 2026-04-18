#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define VECTOR_SIZE 1000000

int main(int argc, char *argv[]) {
    int rank, size;
    int *A, *B;
    int *local_A, *local_B;
    long long local_dot = 0, global_dot = 0;
    double start, end;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int chunk_size = VECTOR_SIZE / size;

    /* All processes allocate to keep MPI_Scatter happy */
    A = (int *)malloc(VECTOR_SIZE * sizeof(int));
    B = (int *)malloc(VECTOR_SIZE * sizeof(int));
    if (rank == 0) {
        for (int i = 0; i < VECTOR_SIZE; i++) {
            A[i] = i + 1;
            B[i] = VECTOR_SIZE - i;
        }
    }

    local_A = (int *)malloc(chunk_size * sizeof(int));
    local_B = (int *)malloc(chunk_size * sizeof(int));

    MPI_Barrier(MPI_COMM_WORLD);
    start = MPI_Wtime();

    MPI_Scatter(A, chunk_size, MPI_INT,
                local_A, chunk_size, MPI_INT,
                0, MPI_COMM_WORLD);

    MPI_Scatter(B, chunk_size, MPI_INT,
                local_B, chunk_size, MPI_INT,
                0, MPI_COMM_WORLD);

    for (int i = 0; i < chunk_size; i++)
        local_dot += (long long)local_A[i] * local_B[i];

    MPI_Reduce(&local_dot, &global_dot, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    end = MPI_Wtime();

    if (rank == 0)
        printf("Processes: %d, Time: %.6f sec, Dot: %lld\n", size, end - start, global_dot);

    free(local_A);
    free(local_B);
    free(A);
    free(B);

    MPI_Finalize();
    return 0;
}
