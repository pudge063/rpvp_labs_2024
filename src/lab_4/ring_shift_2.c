#include <mpi.h>
#include <stdio.h>

int main(int argc, char **argv)
{

    /*
    нулевой процесс инициирует передачу данных
    данные последовательно проходят через все процессы
    данные возвращаются обратно к нулевому процессу
    */

    int rank, size;
    int data = 0;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0)
    {
        data = 100; // начальное значение
        printf("OUTPUT: Процесс 0 отправляет: %d\n", data);
        MPI_Send(&data, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
    }
    else
    {
        // получаем данные от предыдущего процесса
        MPI_Recv(&data, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &status);
        printf("OUTPUT: Процесс %d получил: %d\n", rank, data);

        data += 1;

        // отправляем данные следующему процессу
        if (rank < size - 1)
        {
            MPI_Send(&data, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
        }
        else
        {
            // последний процесс отправляет данные обратно нулевому
            MPI_Send(&data, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        }
    }

    // нулевой процесс получает данные обратно
    if (rank == 0)
    {
        MPI_Recv(&data, 1, MPI_INT, size - 1, 0, MPI_COMM_WORLD, &status);
        printf("OUTPUT: Процесс 0 завершил с получением: %d\n", data);
    }

    MPI_Finalize();
    return 0;
}
