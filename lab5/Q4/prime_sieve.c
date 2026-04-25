#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_NUM 10000

int is_prime(int n) {
    if (n <= 1) return 0;
    for (int i = 2; i * i <= n; i++) {
        if (n % i == 0) return 0;
    }
    return 1;
}

int main(int argc, char** argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        int current_num = 2;
        int active_slaves = size - 1;
        int primes_found = 0;
        
        while (active_slaves > 0) {
            int msg;
            MPI_Status status;
            MPI_Recv(&msg, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            
            if (msg > 0) {
                printf("Found prime: %d\n", msg);
                primes_found++;
            }
            
            if (current_num <= MAX_NUM) {
                MPI_Send(&current_num, 1, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
                current_num++;
            } else {
                int terminate = -1;
                MPI_Send(&terminate, 1, MPI_INT, status.MPI_SOURCE, 1, MPI_COMM_WORLD);
                active_slaves--;
            }
        }
        printf("Total primes found: %d\n", primes_found);
    } else {
        int ready = 0;
        MPI_Send(&ready, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        
        while (1) {
            int num;
            MPI_Status status;
            MPI_Recv(&num, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            
            if (status.MPI_TAG == 1) { // Terminate tag
                break;
            }
            
            if (is_prime(num)) {
                MPI_Send(&num, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
            } else {
                int not_prime = -2;
                MPI_Send(&not_prime, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
            }
        }
    }

    MPI_Finalize();
    return 0;
}
