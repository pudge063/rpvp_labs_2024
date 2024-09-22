#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

void initialize_matrix(float *A, int m, int n)
{
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            A[i * n + j] = rand() % 10 + 1; // Изменено на случайные значения от 1 до 10
        }
    }
}

void initialize_vector(float *B, int n)
{
    for (int j = 0; j < n; j++)
    {
        B[j] = rand() % 10 + 1; // Изменено на случайные значения от 1 до 10
    }
}

int main(int argc, char *argv[])
{
    int rank, size, m = 8, n = 8; // Примерные размеры
    float *A = NULL;              // Полная матрица
    float *full_A = NULL;         // Матрица для броадкаста
    float *local_A = NULL;        // Локальная матрица
    float *B = NULL;              // Вектор
    float *C = NULL;              // Результат
    float *local_C = NULL;        // Локальный результат

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size > m)
    {
        if (rank == 0)
        {
            printf("Ошибка: количество процессов не может превышать количество строк матрицы.\n");
        }
        MPI_Finalize();
        return 1;
    }

    srand(time(NULL) + rank);

    if (rank == 0)
    {
        full_A = (float *)malloc(m * n * sizeof(float));
        B = (float *)malloc(n * sizeof(float));
        C = (float *)malloc(m * sizeof(float));
        initialize_matrix(full_A, m, n);
        initialize_vector(B, n);

        printf("OUTPUT: Матрица (%d x %d):\n", m, n);
        for (int i = 0; i < m; i++)
        {
            for (int j = 0; j < n; j++)
            {
                printf("%8.2f ", full_A[i * n + j]);
            }
            printf("\n");
        }

        printf("OUTPUT: Вектор B: ");
        for (int j = 0; j < n; j++)
        {
            printf("%f ", B[j]);
        }
        printf("\n");
    }

    MPI_Bcast(full_A, m * n, MPI_FLOAT, 0, MPI_COMM_WORLD);
    B = (float *)malloc(n * sizeof(float));
    MPI_Bcast(B, n, MPI_FLOAT, 0, MPI_COMM_WORLD);

    local_A = (float *)malloc((m / size) * n * sizeof(float));
    MPI_Scatter(full_A, (m / size) * n, MPI_FLOAT, local_A, (m / size) * n, MPI_FLOAT, 0, MPI_COMM_WORLD);

    local_C = (float *)calloc(m / size, sizeof(float)); // Инициализация локального результата нулями

    for (int i = 0; i < m / size; i++)
    {
        for (int j = 0; j < n; j++)
        {
            local_C[i] += local_A[i * n + j] * B[j];
            printf("OUTPUT (Process %d): local_A[%d][%d] = %f, B[%d] = %f, local_C[%d] += %f\n",
                   rank, i, j, local_A[i * n + j], j, B[j], i, local_A[i * n + j] * B[j]);
        }
    }

    C = (float *)calloc(m, sizeof(float)); // Инициализация полного результата нулями
    MPI_Gather(local_C, m / size, MPI_FLOAT, C, m / size, MPI_FLOAT, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        printf("OUTPUT: Результат умножения матрицы на вектор:\n");
        for (int i = 0; i < m; i++)
        {
            printf("OUTPUT: C[%d] = %f\n", i, C[i]);
        }
        free(full_A);
        free(B);
        free(C);
    }

    free(local_A);
    free(local_C);
    MPI_Finalize();
    return 0;
}
