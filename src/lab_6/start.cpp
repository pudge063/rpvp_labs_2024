#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

const int N1 = 8, M1 = 4; // размеры матрицы A
const int M2 = 4, N2 = 4; // размеры матрицы B
static double A[N1][M1] = {
    {1, 2, 3, 4},
    {5, 6, 7, 8},
    {9, 10, 11, 12},
    {13, 14, 15, 16},
    {17, 18, 19, 20},
    {21, 22, 23, 24}};

static double B[M2][N2] = {
    {1, 0, 1, 0},
    {0, 1, 0, 1},
    {1, 0, 1, 0},
    {0, 1, 0, 1}};

static double C[N1][N2] = {0}; // результирующая матрица

int main(int argc, char **argv)
{
    // объявление переменных
    int rank, size, i, j, k, i1, sour, dest;
    int N = N1, M = M1;
    double time1, time2;
    double *Avector, *Bvector;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0)
    {
        printf("OUTPUT: Матрица A размером %dx%d\n", N1, M1);
        printf("OUTPUT: Матрица B размером %dx%d\n", N2, M2);
    }

    // Проверка на количество процессов
    if (size > N1)
    {
        if (rank == 0)
        {
            fprintf(stderr, "OUTPUT: количество процессов (%d) превышает количество строк матрицы A (%d).\n", size, N1);
        }
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    if (size > M2)
    {
        if (rank == 0)
        {
            fprintf(stderr, "OUTPUT: количество процессов (%d) превышает количество строк матрицы B (%d).\n", size, M2);
        }
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    // проверка на возможность деления строк матрицы A
    if (N1 % size != 0)
    {
        if (rank == 0)
        {
            fprintf(stderr, "OUTPUT: число процессов должно делиться на количество строк матрицы A (%d).\n", N1);
        }
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    N = N1 / size; // делим строки матрицы A поровну

    Avector = new double[N * M1]; // динамические массивы для полос матриц A и B
    Bvector = new double[M2 * N2];

    // заполнение полос матриц
    for (i = 0; i < N; i++)
    {
        for (int j = 0; j < M1; j++)
        {
            Avector[i * M1 + j] = A[rank * N + i][j];
        }
    }

    // транспонируем матрицу B
    for (i = 0; i < M2; i++)
    {
        for (int j = 0; j < N2; j++)
        {
            Bvector[i * N2 + j] = B[j][i];
        }
    }

    time1 = MPI_Wtime();

    // основной цикл по процессам для пересылки полос B
    for (k = 0; k < size; k++)
    {
        // сбрасываем значения в результирующей матрице C
        for (i = 0; i < N; i++)
        {
            for (j = 0; j < N2; j++)
            {
                C[i][j] = 0.0;
            }
        }

        for (i = 0; i < N; i++) // перебор строк полосы матрицы A
        {
            for (j = 0; j < N2; j++) // перебор столбцов матрицы C
            {
                for (i1 = 0; i1 < M1; i1++) // цикл умножения элемента строки A на элемент столбца B
                {
                    C[i][j] += Avector[i * M1 + i1] * Bvector[j * M2 + i1];
                }
            }
        }

        // вычисление соседей по кольцу
        sour = (rank - 1 + size) % size; // левый сосед
        dest = (rank + 1) % size;        // правый сосед

        // передаем полосы матрицы B соседям по кольцу
        MPI_Sendrecv_replace(Bvector, M2 * N2, MPI_DOUBLE, dest, 12, sour, 12, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    time2 = MPI_Wtime();

    // Сборка результата
    if (rank == 0)
    {
        for (int k = 0; k < size; k++)
        {
            if (k != 0)
            {
                MPI_Recv(C + k * N, N * N2, MPI_DOUBLE, k, 5, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
        }

        // Вывод результирующей матрицы C
        printf("OUTPUT: Результирующая матрица C:\n");
        for (i = 0; i < N1; i++)
        {
            printf("OUTPUT: ");
            for (j = 0; j < N2; j++)
            {
                printf("%f ", C[i][j]);
            }
            printf("\n");
        }
    }
    else
    {
        MPI_Send(C, N * N2, MPI_DOUBLE, 0, 5, MPI_COMM_WORLD);
    }

    delete[] Avector;
    delete[] Bvector;

    MPI_Finalize();
    return 0;
}
