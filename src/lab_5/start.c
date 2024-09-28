#include <mpi.h>
#include <stdio.h>
#define NUM_DIMS 1

int main(int argc, char **argv)
{
    int rank, size, i, A, B, dims[NUM_DIMS];
    int periods[NUM_DIMS], new_coords[NUM_DIMS];
    int sourcem, destb;
    int reorder = 0;
    MPI_Comm comm_cart;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // инициализация массива dims и periods для топологии "линейка"
    for (i = 0; i < NUM_DIMS; i++)
    {
        dims[i] = 0;
        periods[i] = 0; // непериодическая топология
    }

    // заполняем массив dims
    MPI_Dims_create(size, NUM_DIMS, dims);

    // создаем топологию "линейка" с communicator-ом comm_cart
    MPI_Cart_create(MPI_COMM_WORLD, NUM_DIMS, dims, periods, reorder, &comm_cart);

    // отображаем ранги в координаты и выводим их
    MPI_Cart_coords(comm_cart, rank, NUM_DIMS, new_coords);
    A = new_coords[0]; // инициализируем данные с номером процесса
    B = -1;

    // определяем соседей
    int left = new_coords[0] - 1; // левый сосед
    int right = new_coords[0] + 1; // правый сосед

    // сдвиг данных вдоль линейки в сторону увеличения
    if (right < dims[0]) {
        MPI_Sendrecv(&A, 1, MPI_INT, right, 0, &B, 1, MPI_INT, left, 0, comm_cart, &status); // отправка и получение данных
        printf("OUTPUT: rank=%d отправил %d, получил %d от %d\n", rank, A, B, left);
        A = B; // обновляем значение A
    } else {
        printf("OUTPUT: rank=%d (крайний процесс) не отправил в правый процесс\n", rank);
    }

    // сдвиг данных вдоль линейки в сторону уменьшения
    if (left >= 0) {
        MPI_Sendrecv(&A, 1, MPI_INT, left, 0, &B, 1, MPI_INT, right, 0, comm_cart, &status); // отправка и получение данных
        printf("OUTPUT: rank=%d отправил %d, получил %d от %d\n", rank, A, B, right);
        A = B; // обновляем значение A
    } else {
        printf("OUTPUT: rank=%d (крайний процесс) не отправил в левый процесс\n", rank);
    }

    // освобождаем ресурсы
    MPI_Comm_free(&comm_cart);
    MPI_Finalize();
    return 0;
}
