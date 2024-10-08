#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define COUNT_1B 1
#define COUNT_1KB 1024
#define COUNT_1MB (1024 * 1024)

// сообщения передаются по кольцу (процесс n получает сообщение от процесса n - 1 и отправляет сообщение процессу n + 1)

void ring_exchange(int size, int count, char *message) // количество процессов, размер сообщения в байтах, блок в памяти для передачи сообщения
{
    int rank, num_procs, prev, next;
    MPI_Status status;
    double start_time, end_time, elapsed_time;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);      // получить ранг текущего процесса из коммуникатора
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs); // получения общего числа процессов в коммуникаторе

    prev = (rank - 1 + num_procs) % num_procs; // определение предыдущего и следующего ранга процесса в колцье
    next = (rank + 1) % num_procs;

    if (rank == 0) // начать измерение времени на 0 процессе
    {
        start_time = MPI_Wtime();
    }

    for (int i = 0; i < num_procs - 1; i++) // передача сообщения из предыдущего в следующий процесс
    {
        if (rank == 0)
        {
            MPI_Send(message, count, MPI_CHAR, next, 0, MPI_COMM_WORLD);          // процесс отправляет сообщение следующему
            MPI_Recv(message, count, MPI_CHAR, prev, 0, MPI_COMM_WORLD, &status); // процесс получает сообщение от предыдущего
            printf("OUTPUT: PROCESS %d SENT MESSAGE TO PROCESS %d IN ITERATION %d\n", rank, next, i);
        }
        else
        {
            MPI_Recv(message, count, MPI_CHAR, prev, 0, MPI_COMM_WORLD, &status);
            MPI_Send(message, count, MPI_CHAR, next, 0, MPI_COMM_WORLD);
            // printf("OUTPUT: to=%d rank=%d", next, i);
            printf("OUTPUT: PROCESS %d SENT MESSAGE TO PROCESS %d IN ITERATION %d\n", rank, next, i);
        }
    }

    if (rank == 0) // вычисление времени после прохождения круга
    {
        end_time = MPI_Wtime();
        elapsed_time = end_time - start_time;
        printf("OUTPUT: Количество процессов: %d, Размер сообщения: %d байт, Время: %f секунд\n", num_procs, count, elapsed_time);
    }
}

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);

    int rank, num_procs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    char *message = (char *)malloc(COUNT_1MB * sizeof(char));

    ring_exchange(num_procs, COUNT_1B, message);
    ring_exchange(num_procs, COUNT_1KB, message);
    ring_exchange(num_procs, COUNT_1MB, message);

    free(message);
    MPI_Finalize(); // конец фукнции
    return 0;
}
