#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define ARRAY_SIZE 100
#define NUM_PROCS  4
#define CHUNK_SIZE (ARRAY_SIZE / NUM_PROCS)  /* 25 */

int main(int argc, char *argv[]) {
    int rank, size;
    int array[ARRAY_SIZE];
    int local_array[CHUNK_SIZE];
    int local_sum = 0, global_sum = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size != NUM_PROCS) {
        if (rank == 0)
            printf("Error: This program requires exactly %d processes.\n", NUM_PROCS);
        MPI_Finalize();
        return 1;
    }

    /* Process 0 initializes the array with values 1..100 */
    if (rank == 0) {
        printf("Process 0: Initializing array[100] = {1, 2, 3, ..., 100}\n");
        for (int i = 0; i < ARRAY_SIZE; i++)
            array[i] = i + 1;
    }

    /* Scatter 25 elements to each process */
    MPI_Scatter(array, CHUNK_SIZE, MPI_INT,
                local_array, CHUNK_SIZE, MPI_INT,
                0, MPI_COMM_WORLD);

    /* Each process computes local sum */
    for (int i = 0; i < CHUNK_SIZE; i++)
        local_sum += local_array[i];

    printf("Process %d: Local sum = %d (elements %d to %d)\n",
           rank, local_sum, rank * CHUNK_SIZE + 1, (rank + 1) * CHUNK_SIZE);

    /* Reduce all local sums to global sum on process 0 */
    MPI_Reduce(&local_sum, &global_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("Process 0: Global sum = %d\n", global_sum);
        printf("Process 0: Average = %.1f\n", (double)global_sum / ARRAY_SIZE);
    }

    MPI_Finalize();
    return 0;
}
