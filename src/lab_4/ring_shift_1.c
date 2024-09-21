#include <mpi.h>
#include <stdio.h>
#define NUM_DIMS 1

int main(int argc, char **argv)
{
    int rank, size, i, A, B;
    int dims[NUM_DIMS];
    int periods[NUM_DIMS], source, dest;
    int reorder = 0;
    MPI_Comm comm_cart;
    MPI_Status status;

    // Инициализация MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Инициализация переменной A значением ранга
    A = rank;
    B = -1;

    // Обнуляем массив dims и заполняем массив periods для топологии "кольцо"
    for (i = 0; i < NUM_DIMS; i++)
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
    MPI_Sendrecv(&A, 1, MPI_INT, dest, 0, &B, 1, MPI_INT, source, 0, comm_cart, &status);

    // Вывод информации о ранге и полученном значении B
    printf("OUTPUT: rank=%d B=%d\n", rank, B);

    // Освобождаем ресурсы
    MPI_Comm_free(&comm_cart);
    MPI_Finalize();
    return 0;
}
