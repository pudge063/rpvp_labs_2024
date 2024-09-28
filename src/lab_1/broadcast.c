#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define COUNT_1KB 1024
#define COUNT_1MB (1024 * 1024)

void manual_broadcast(int size, int count)
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
        for (int i = 0; i < count; i++)
        {
            buffer[i] = 'A';
        }
        start_time = MPI_Wtime();

        for (int i = 1; i < num_procs; i++)
        {
            MPI_Send(buffer, count, MPI_CHAR, i, 0, MPI_COMM_WORLD);
        }

        printf("OUTPUT: PROCESS %d SENT MESSAGE TO ALL PROCESSES\n", rank);
    }
    else
    {
        MPI_Recv(buffer, count, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);
        printf("OUTPUT: PROCESS %d RECEIVED MESSAGE FROM PROCESS 0\n", rank);
    }

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

    free(buffer);
}

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);

    int rank, num_procs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    manual_broadcast(num_procs, COUNT_1KB);
    manual_broadcast(num_procs, COUNT_1MB);

    MPI_Finalize();
    return 0;
}
