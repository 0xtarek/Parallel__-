#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>

int is_prime(int n) {
    if (n <= 1) {
        return 0;
    }
    int i;
    for (i = 2; i <= sqrt(n); i++) {
        if (n % i == 0) {
            return 0;
        }
    }
    return 1;
}



int main(int argc, char** argv) {
    int rank, size;
    double start_time, end_time;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        start_time = MPI_Wtime();
    }
    
    int x, y;
    int r = 0; // subrange size
    int i;
    int count = 0; // count of primes in the subrange
    
    if (rank == 0) {
        printf("Enter lower bound and upper bound separated by a space: ");
        scanf("%d %d", &x, &y);
        r = (y - x) / (size - 1);
        
        // broadcast x and r to each slave process
        for (i = 1; i < size; i++) {
            MPI_Send(&x, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&r, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
        
        // calculate count of primes in the master's subrange
        for (i = x; i <= x + r; i++) {
            count += is_prime(i);
        }
        
        // receive sub-count from each slave process
        int sub_count;
        for (i = 1; i < size; i++) {
            MPI_Recv(&sub_count, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            count += sub_count;
        }
        
        // print total count of primes between x and y
        printf("Total count of primes between %d and %d is %d\n", x, y, count);
    } else {
        // receive x and r from the master
        MPI_Recv(&x, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&r, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
        // calculate the lower and upper bounds of the subrange for the slave
        int a = x + (rank - 1) * r + 1;
        int b = a + r - 1;
        if (rank == size - 1) {
            b = y;
        }
        
        // count primes in the subrange
        for (i = a; i <= b; i++) {
            count += is_prime(i);
        }
        
        // send sub-count to the master
        MPI_Send(&count, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }
    if (rank == 0) 
    {
        end_time = MPI_Wtime();
        printf("MPI_Send and MPI_Recv method took %f seconds\n", end_time - start_time);
    }
    MPI_Finalize();
    return 0;
}