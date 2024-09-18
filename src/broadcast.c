#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define COUNT_1KB 1024
#define COUNT_1MB (1024 * 1024)

void broadcast_test(int size, int count)
{
    int rank, num_procs;
    MPI_Status status;
    double start_time, end_time, elapsed_time;
    char *buffer;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    buffer = (char *)malloc(count * sizeof(char));

    if (rank == 0)
    {
        start_time = MPI_Wtime();
    }

    MPI_Bcast(buffer, count, MPI_CHAR, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        end_time = MPI_Wtime();
        elapsed_time = end_time - start_time;
        printf("OUTPUT: Number of processes: %d, Message size: %d bytes, Time: %f seconds\n", num_procs, count, elapsed_time);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == 0)
    {
        printf("OUTPUT: All processes have reached the barrier and finished broadcasting.\n");
    }

    free(buffer);
}

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);

    int rank, num_procs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    broadcast_test(num_procs, COUNT_1KB);
    broadcast_test(num_procs, COUNT_1MB);

    MPI_Finalize();
    return 0;
}
