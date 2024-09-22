#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define COUNT_1KB 1024
#define COUNT_1MB (1024 * 1024)

// реализация функции MPI_Bcast - процесс с рангом 0 передаёт сообщения всем остальынм процессам

void broadcast_test(int size, int count)
{
    int rank, num_procs;
    MPI_Status status;
    double start_time, end_time, elapsed_time;
    char *buffer;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    buffer = (char *)malloc(count * sizeof(char)); // буфер сообщения

    if (rank == 0)
    {
        start_time = MPI_Wtime();
    }

    // реализация broadcast: буфер, передаваемые данные, тип данных, ранг процесса инициатора, коммуникатор
    MPI_Bcast(buffer, count, MPI_CHAR, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        end_time = MPI_Wtime();
        elapsed_time = end_time - start_time;
        printf("OUTPUT: Номер процесса: %d, Размер сообщения: %d байт, Время: %f секунд\n", num_procs, count, elapsed_time);
    }

    MPI_Barrier(MPI_COMM_WORLD); // синхронизация всех процессов через MPI_Barrier

    if (rank == 0)
    {
        printf("OUTPUT: Все процессы завершились в одно время.\n");
    }

    // free(buffer);
}

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);

    // int rank, num_procs;
    // MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    // MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    broadcast_test(num_procs, COUNT_1KB);
    broadcast_test(num_procs, COUNT_1MB);

    MPI_Finalize();
    return 0;
}
