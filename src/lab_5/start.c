#include <mpi.h>
#include <stdio.h>
#define NUM_DIMS 1

int main(int argc, char **argv)
{

    /*
    сдвиг данных вдоль линейки компьютеров.
    Все ветви должны одновременно сдвигать свои данные соседним ветвям вдоль линейки 
    на один шаг в сторону увеличения и, затем, в сторону уменьшения ранга ветвей.
    */

    int rank, size, i, A, B, dims[NUM_DIMS];
    int periods[NUM_DIMS], new_coords[NUM_DIMS];
    int sourceb, destb, sourcem, destm;
    int reorder = 0;
    MPI_Comm comm_cart;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // обнуляем массив dims и заполняем массив periods для топологии "линейка"
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
    sourcem = (new_coords[0] == 0) ? MPI_PROC_NULL : new_coords[0] - 1;
    destb = (new_coords[0] == dims[0] - 1) ? MPI_PROC_NULL : new_coords[0] + 1;

    // сдвиг данных вдоль линейки в сторону увеличения
    MPI_Sendrecv(&A, 1, MPI_INT, destb, 0, &B, 1, MPI_INT, sourcem, 0, comm_cart, &status); // отправка и получение данных
    printf("OUTPUT: rank=%d отправил %d, получил %d от %d\n", rank, A, B, sourcem);

    // сдвиг данных вдоль линейки в сторону уменьшения
    sourcem = (new_coords[0] == 0) ? MPI_PROC_NULL : new_coords[0] - 1;
    destb = (new_coords[0] == dims[0] - 1) ? MPI_PROC_NULL : new_coords[0] + 1;

    MPI_Sendrecv(&A, 1, MPI_INT, sourcem, 0, &B, 1, MPI_INT, destb, 0, comm_cart, &status); // отправка и получение данных
    printf("OUTPUT: rank=%d отправил %d, получил %d от %d\n", rank, A, B, destb);

    // ссвобождаем ресурсы
    MPI_Comm_free(&comm_cart);
    MPI_Finalize();
    return 0;
}
