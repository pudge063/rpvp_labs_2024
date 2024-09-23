#include <stdio.h>
#include <mpi.h>

#define NUM_DIMS 1

// const int N1 = 8, M1 = 8;
// static double A[N1][M1] = {
//     {1, 0, 0, 0, 0, 0, 0, 0},
//     {0, 1, 0, 0, 0, 0, 0, 0},
//     {0, 0, 1, 0, 0, 0, 0, 0},
//     {0, 0, 0, 1, 0, 0, 0, 0},
//     {0, 0, 0, 0, 1, 0, 0, 0},
//     {0, 0, 0, 0, 0, 1, 0, 0},
//     {0, 0, 0, 0, 0, 0, 1, 0},
//     {0, 0, 0, 0, 0, 0, 0, 1}};

// static double B[M1][N1] = {
//     {1, 2, 3, 4, 5, 6, 7, 8},
//     {2, 3, 4, 5, 6, 7, 8, 9},
//     {3, 4, 5, 6, 7, 8, 9, 10},
//     {4, 5, 6, 7, 8, 9, 10, 11},
//     {5, 6, 7, 8, 9, 10, 11, 12},
//     {6, 7, 8, 9, 10, 11, 12, 13},
//     {7, 8, 9, 10, 11, 12, 13, 14},
//     {8, 9, 10, 11, 12, 13, 14, 15}};

const int N1 = 8, M1=8;
static double A[N1][M1] = {
    {1, 2, 3, 4, 5, 6, 7, 8},
    {2, 3, 4, 5, 6, 7, 8, 9},
    {3, 4, 5, 6, 7, 8, 9, 10},
    {4, 5, 6, 7, 8, 9, 10, 11},
    {5, 6, 7, 8, 9, 10, 11, 12},
    {6, 7, 8, 9, 10, 11, 12, 13},
    {7, 8, 9, 10, 11, 12, 13, 14},
    {8, 9, 10, 11, 12, 13, 14, 15}};

static double B[N1][M1] = {
    {1, 0, 1, 0, 1, 0, 1, 0},
    {0, 1, 0, 1, 0, 1, 0, 1},
    {1, 0, 1, 0, 1, 0, 1, 0},
    {0, 1, 0, 1, 0, 1, 0, 1},
    {1, 0, 1, 0, 1, 0, 1, 0},
    {0, 1, 0, 1, 0, 1, 0, 1},
    {1, 0, 1, 0, 1, 0, 1, 0},
    {0, 1, 0, 1, 0, 1, 0, 1}};

static double C[N1][N1] = {0}; // матрица результата умножения

int main(int argc, char **argv)
{
    // объявление всех переменных
    int rank, size, i, j, k, i1, j1, d, sour, dest;
    int N = N1, M = M1;
    int dims[NUM_DIMS], periods[NUM_DIMS], new_coords[NUM_DIMS];
    int reorder = 0;
    MPI_Comm comm_cart;
    MPI_Status st;
    double time1, time2;
    double *Avector, *Bvector;
    int dt1;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    N = N1 / size; // делим строки матрицы A поровну

    Avector = new double[N * M]; // динамические массивы для полос матриц A и B
    Bvector = new double[N * M];

    // заполнение полос матриц
    for (i = 0; i < N; i++)
    {
        for (j = 0; j < M; j++)
        {
            Avector[i * M + j] = A[rank * N + i][j];
            Bvector[i * M + j] = B[j][rank * N + i]; // транспонирование
        }
    }

    // обнуление массива dims и заполнение массива periods для топологии кольцо
    for (i = 0; i < NUM_DIMS; i++)
    {
        dims[i] = 0;
        periods[i] = 1;
    }

    MPI_Dims_create(size, NUM_DIMS, dims);
    MPI_Cart_create(MPI_COMM_WORLD, NUM_DIMS, dims, periods, reorder, &comm_cart); // создание топологии одномерная карта
    MPI_Cart_coords(comm_cart, rank, NUM_DIMS, new_coords);
    MPI_Cart_shift(comm_cart, 0, -1, &sour, &dest);

    time1 = MPI_Wtime();

    // основной цикл по процессам для пересылки полос B
    for (k = 0; k < size; k++) // итерации, в которых каждый процесс умножает свою полосу A на полосу B, k - номер процесса
    {
        for (i = 0; i < N; i++) // перебор строк полосы матрцы A
        {
            for (j = 0; j < N; j++) // перебор столбцов матрицы C
            {
                for (i1 = 0; i1 < M; i1++) // цикл умножения элемента строки A на элемент столбца B
                {
                    C[i][j + ((rank + k) % size) * N] += Avector[i * M + i1] * Bvector[j * M + i1]; // добавление в итоговую матрицу результата перемножения полосок
                }
            }
        }

        // передаем полосы матрицы B соседям по кольцу
        MPI_Sendrecv_replace(Bvector, N * M, MPI_DOUBLE, dest, 12, sour, 12, comm_cart, &st);
    }

    time2 = MPI_Wtime();
    dt1 = (int)(time2 - time1);

    printf("OUTPUT: Процесс %d завершил за %d секунд.\n", rank, dt1);

    // собираем матрицу C в процессе 0
    if (rank != 0)
    {
        // отправка результатов со всех процессов к процессу 0
        MPI_Send(C, N * N * size, MPI_DOUBLE, 0, 5, MPI_COMM_WORLD);
    }
    else
    {
        // на процессе 0 получаем все сообщения с результатами
        for (k = 1; k < size; k++)
        {
            MPI_Recv(C + k * N, N * N * size, MPI_DOUBLE, k, 5, MPI_COMM_WORLD, &st);
        }

        // вывод результирующей матрицы C
        printf("OUTPUT: Результирующая матрица C:\n");
        for (i = 0; i < N1; i++)
        {
            for (j = 0; j < N1; j++)
            {
                printf("OUTPUT: %f ", C[i][j]);
            }
            printf("\n");
        }
    }

    delete[] Avector;
    delete[] Bvector;

    MPI_Comm_free(&comm_cart);
    MPI_Finalize();
    return 0;
}
