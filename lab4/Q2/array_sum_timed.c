#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define ARRAY_SIZE 1000000

int main(int argc, char *argv[]) {
    int rank, size;
    int *array;
    int *local_array;
    long long local_sum = 0, global_sum = 0;
    double start, end;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int chunk_size = ARRAY_SIZE / size;

    /* All processes allocate the full array to keep MPI_Scatter happy */
    array = (int *)malloc(ARRAY_SIZE * sizeof(int));
    if (rank == 0) {
        for (int i = 0; i < ARRAY_SIZE; i++)
            array[i] = i + 1;
    }

    local_array = (int *)malloc(chunk_size * sizeof(int));

    MPI_Barrier(MPI_COMM_WORLD);
    start = MPI_Wtime();

    MPI_Scatter(array, chunk_size, MPI_INT,
                local_array, chunk_size, MPI_INT,
                0, MPI_COMM_WORLD);

    for (int i = 0; i < chunk_size; i++)
        local_sum += local_array[i];

    MPI_Reduce(&local_sum, &global_sum, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    end = MPI_Wtime();

    if (rank == 0)
        printf("Processes: %d, Time: %.6f sec, Sum: %lld\n", size, end - start, global_sum);

    free(local_array);
    free(array);

    MPI_Finalize();
    return 0;
}
