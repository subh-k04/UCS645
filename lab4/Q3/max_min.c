#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define NUM_ELEMENTS 10
#define MAX_VAL 1000

int main(int argc, char *argv[]) {
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    /* Seed with rank so each process gets different random numbers */
    srand(rank + 1);

    int data[NUM_ELEMENTS];
    printf("Process %d: Generated numbers: ", rank);
    for (int i = 0; i < NUM_ELEMENTS; i++) {
        data[i] = rand() % (MAX_VAL + 1);
        printf("%d ", data[i]);
    }
    printf("\n");

    /* Find local max and min */
    int local_max = data[0], local_min = data[0];
    for (int i = 1; i < NUM_ELEMENTS; i++) {
        if (data[i] > local_max) local_max = data[i];
        if (data[i] < local_min) local_min = data[i];
    }

    printf("Process %d: Local max = %d, Local min = %d\n", rank, local_max, local_min);

    /* Use MPI_2INT with MPI_MAXLOC and MPI_MINLOC */
    /* MPI_2INT expects pairs of {value, rank} */
    int local_max_pair[2] = {local_max, rank};
    int local_min_pair[2] = {local_min, rank};
    int global_max_pair[2], global_min_pair[2];

    MPI_Reduce(local_max_pair, global_max_pair, 1, MPI_2INT, MPI_MAXLOC, 0, MPI_COMM_WORLD);
    MPI_Reduce(local_min_pair, global_min_pair, 1, MPI_2INT, MPI_MINLOC, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("\n--- Global Results ---\n");
        printf("Global Maximum: %d (found on Process %d)\n", global_max_pair[0], global_max_pair[1]);
        printf("Global Minimum: %d (found on Process %d)\n", global_min_pair[0], global_min_pair[1]);
    }

    MPI_Finalize();
    return 0;
}
