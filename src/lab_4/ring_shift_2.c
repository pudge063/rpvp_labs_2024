#include <mpi.h>
#include <stdio.h>
#define NUM_DIMS 1

int main(int argc, char **argv)
{
    int rank, size;
    int dims[NUM_DIMS];
    int periods[NUM_DIMS], source, dest;
    int reorder = 0;
    MPI_Comm comm_cart;
    MPI_Status status;

    // Инициализация MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Инициализация переменной для передачи
    int data;
    if (rank == 0)
    {
        data = 100; // Начальное значение от нулевого процесса
        printf("Процесс 0 отправляет: %d\n", data);
    }

    // Обнуляем массив dims и заполняем массив periods для топологии "кольцо"
    for (int i = 0; i < NUM_DIMS; i++)
    {
        dims[i] = 0;
        periods[i] = 1; // Периодическая топология
    }

    // Заполняем массив dims
    MPI_Dims_create(size, NUM_DIMS, dims);

    // Создаем топологию "кольцо" с communicator-ом comm_cart
    MPI_Cart_create(MPI_COMM_WORLD, NUM_DIMS, dims, periods, reorder, &comm_cart);

    // Каждая ветвь находит своих соседей вдоль кольца
    MPI_Cart_shift(comm_cart, 0, 1, &source, &dest);

    // Передача данных вдоль кольца
    MPI_Sendrecv(&data, 1, MPI_INT, dest, 0, &data, 1, MPI_INT, source, 0, comm_cart, &status);

    // Вывод информации о ранге и полученном значении
    printf("OUTPUT: rank=%d получил: %d\n", rank, data);

    // Нулевой процесс принимает данные обратно и завершает выполнение
    if (rank == 0)
    {
        printf("OUTPUT: Процесс 0 завершил с получением: %d\n", data);
    }

    // Освобождаем ресурсы
    MPI_Comm_free(&comm_cart);
    MPI_Finalize();
    return 0;
}
