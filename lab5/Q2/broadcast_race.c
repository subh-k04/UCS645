#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define N 10000000

int main(int argc, char** argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    double *data = (double*)malloc(N * sizeof(double));
    if (rank == 0) {
        for (int i = 0; i < N; i++) data[i] = 1.0;
    }

    // Part A: MyBcast
    MPI_Barrier(MPI_COMM_WORLD);
    double t1 = MPI_Wtime();
    if (rank == 0) {
        for (int i = 1; i < size; i++) {
            MPI_Send(data, N, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
        }
    } else {
        MPI_Recv(data, N, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    double t2 = MPI_Wtime();
    double mybcast_time = t2 - t1;

    // Part B: MPI_Bcast
    MPI_Barrier(MPI_COMM_WORLD);
    double t3 = MPI_Wtime();
    MPI_Bcast(data, N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    double t4 = MPI_Wtime();
    double bcast_time = t4 - t3;

    if (rank == 0) {
        printf("MyBcast time: %f sec\n", mybcast_time);
        printf("MPI_Bcast time: %f sec\n", bcast_time);
    }

    free(data);
    MPI_Finalize();
    return 0;
}
