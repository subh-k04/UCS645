#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
    int rank, size;
    int send_val, recv_val;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int next = (rank + 1) % size;
    int prev = (rank - 1 + size) % size;

    if (rank == 0) {
        /* Process 0 starts: send initial value 100 to next */
        send_val = 100 + rank;  /* 100 + 0 = 100 */
        printf("Process %d: Sending %d to process %d\n", rank, send_val, next);
        MPI_Send(&send_val, 1, MPI_INT, next, 0, MPI_COMM_WORLD);

        /* Wait for the value to come back around the ring */
        MPI_Recv(&recv_val, 1, MPI_INT, prev, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Process %d: Received %d from process %d\n", rank, recv_val, prev);
        printf("Process %d: Final value after traversing the ring = %d\n", rank, recv_val);
    } else {
        if (rank % 2 == 1) {
            /* Odd ranks: receive first, then send */
            MPI_Recv(&recv_val, 1, MPI_INT, prev, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("Process %d: Received %d from process %d\n", rank, recv_val, prev);
            send_val = recv_val + rank;
            printf("Process %d: Sending %d to process %d\n", rank, send_val, next);
            MPI_Send(&send_val, 1, MPI_INT, next, 0, MPI_COMM_WORLD);
        } else {
            /* Even ranks (non-zero): receive first then send
               In a ring with standard ordering this is safe */
            MPI_Recv(&recv_val, 1, MPI_INT, prev, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("Process %d: Received %d from process %d\n", rank, recv_val, prev);
            send_val = recv_val + rank;
            printf("Process %d: Sending %d to process %d\n", rank, send_val, next);
            MPI_Send(&send_val, 1, MPI_INT, next, 0, MPI_COMM_WORLD);
        }
    }

    MPI_Finalize();
    return 0;
}
