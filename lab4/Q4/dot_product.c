#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define VECTOR_SIZE 8
#define NUM_PROCS   4
#define CHUNK_SIZE  (VECTOR_SIZE / NUM_PROCS)  /* 2 */

int main(int argc, char *argv[]) {
    int rank, size;
    int A[VECTOR_SIZE] = {1, 2, 3, 4, 5, 6, 7, 8};
    int B[VECTOR_SIZE] = {8, 7, 6, 5, 4, 3, 2, 1};
    int local_A[CHUNK_SIZE], local_B[CHUNK_SIZE];
    int local_dot = 0, global_dot = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size != NUM_PROCS) {
        if (rank == 0)
            printf("Error: This program requires exactly %d processes.\n", NUM_PROCS);
        MPI_Finalize();
        return 1;
    }

    if (rank == 0) {
        printf("Vector A: ");
        for (int i = 0; i < VECTOR_SIZE; i++) printf("%d ", A[i]);
        printf("\nVector B: ");
        for (int i = 0; i < VECTOR_SIZE; i++) printf("%d ", B[i]);
        printf("\n\n");
    }

    /* Scatter chunks of both vectors to all processes */
    MPI_Scatter(A, CHUNK_SIZE, MPI_INT,
                local_A, CHUNK_SIZE, MPI_INT,
                0, MPI_COMM_WORLD);

    MPI_Scatter(B, CHUNK_SIZE, MPI_INT,
                local_B, CHUNK_SIZE, MPI_INT,
                0, MPI_COMM_WORLD);

    /* Compute partial dot product */
    for (int i = 0; i < CHUNK_SIZE; i++)
        local_dot += local_A[i] * local_B[i];

    printf("Process %d: Partial dot product = %d (A[%d..%d] * B[%d..%d])\n",
           rank, local_dot,
           rank * CHUNK_SIZE, rank * CHUNK_SIZE + CHUNK_SIZE - 1,
           rank * CHUNK_SIZE, rank * CHUNK_SIZE + CHUNK_SIZE - 1);

    /* Reduce partial dot products to global result */
    MPI_Reduce(&local_dot, &global_dot, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0)
        printf("\nGlobal Dot Product: A . B = %d\n", global_dot);

    MPI_Finalize();
    return 0;
}
