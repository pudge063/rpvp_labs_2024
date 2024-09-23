#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define COUNT_1KB 1024
#define COUNT_1MB (1024 * 1024)

// сбор данных со всех процессов в один (в данном случае на нулевой)
// каждый процесс генерирует данные и отправляет на нулевой процесс
// MPI barrier гарантирует одновременное начало процессов

void gather_test(int count)
{
    int rank, num_procs;
    double start_time, end_time, elapsed_time;
    char *sbuf, *rbuf = NULL;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);      // определение ранга текущего процесса
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs); // количество процессов в коммуникаторе

    sbuf = (char *)malloc(count * sizeof(char)); // выделение памяти исходного буфера каждого процесса

    if (rank == 0)
    { // процесс 0 получает rbuf - дополнительный буфер для сбора сообщений с других процессах
        rbuf = (char *)malloc(count * num_procs * sizeof(char));
    }

    for (int i = 0; i < count; i++)
    { // генерация сообщения
        sbuf[i] = 'A' + (rank % 26);
    }

    MPI_Barrier(MPI_COMM_WORLD); // все процессы начинаются одновременно
    start_time = MPI_Wtime();    //

    // основная операция
    MPI_Gather(sbuf, count, MPI_CHAR, rbuf, count, MPI_CHAR, 0, MPI_COMM_WORLD); // исходный буфер, размер сообщения байт, тип сообщений, таргет буфер

    MPI_Barrier(MPI_COMM_WORLD);          //
    end_time = MPI_Wtime();               //
    elapsed_time = end_time - start_time; // запись времени выполнения

    if (rank == 0)
    {
        printf("OUTPUT: Сбор сообщений с размером %d байт завершен за %f секунд\n", count, elapsed_time);
    }
    else
    {
        printf("OUTPUT: PROCESS %d: Отправлено %d байт на процесс 0\n", rank, count);
    }

    // освобождение памяти
    free(sbuf);
    if (rank == 0)
    {
        free(rbuf);
    }
}

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);

    gather_test(COUNT_1KB);
    gather_test(COUNT_1MB);

    MPI_Finalize();
    return 0;
}
