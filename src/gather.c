#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define COUNT_1KB 1024
#define COUNT_1MB (1024 * 1024)

void gather_test(int count) {
    int rank, num_procs;
    double start_time, end_time, elapsed_time;
    char *sbuf, *rbuf = NULL;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    sbuf = (char *)malloc(count * sizeof(char));

    if (rank == 0) {
        rbuf = (char *)malloc(count * num_procs * sizeof(char));
    }

    for (int i = 0; i < count; i++) {
        sbuf[i] = 'A' + (rank % 26);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    start_time = MPI_Wtime();

    MPI_Gather(sbuf, count, MPI_CHAR, rbuf, count, MPI_CHAR, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    end_time = MPI_Wtime();
    elapsed_time = end_time - start_time;

    if (rank == 0) {
        printf("OUTPUT: Сбор сообщений с размером %d байт завершен за %f секунд\n", count, elapsed_time);
    }

    free(sbuf);
    if (rank == 0) {
        free(rbuf);
    }
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int rank, num_procs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    gather_test(COUNT_1KB);
    gather_test(COUNT_1MB);

    MPI_Finalize();
    return 0;
}
