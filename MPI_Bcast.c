#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>

int is_prime(int n) {
    if (n <= 1) {
        return 0;
    }
    for (int i = 2; i <= sqrt(n); i++) {
        if (n % i == 0) {
            return 0;
        }
    }
    return 1;
}

int main(int argc, char** argv) {
    int rank, size;
    double start_time, end_time;
    int x, y, r, a, b, sub_count = 0, total_count = 0;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        start_time = MPI_Wtime();
    }

    if (rank == 0) {
        printf("Enter the lower bound x and upper bound y: ");
        scanf("%d %d", &x, &y);
        r = (y - x) / size;
    }

    MPI_Bcast(&x, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&r, 1, MPI_INT, 0, MPI_COMM_WORLD);

    a = x + rank * r;
    b = a + r;

    for (int i = a; i < b; i++) {
        sub_count += is_prime(i);
    }

    MPI_Reduce(&sub_count, &total_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("Total count of prime numbers between %d and %d: %d\n", x, y, total_count);
    }

    if (rank == 0) 
    {
        end_time = MPI_Wtime();
        printf("MPI_Bcast and MPI_Reduce method took %f seconds\n", end_time - start_time);
    }

    MPI_Finalize();
    return 0;
}