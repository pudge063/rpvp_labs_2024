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
            A[i * n + j] = rand() % 10;
        }
    }
}

void print_matrix(float *A, int m, int n)
{
    printf("OUTPUT: Матрица (%d x %d):\n", m, n);
    for (int i = 0; i < m; i++)
    {
        printf("OUTPUT: ");
        for (int j = 0; j < n; j++)
        {
            printf("%8.2f ", A[i * n + j]);
        }
        printf("\n");
    }
    printf("\n");
}

void initialize_vector(float *B, int n)
{
    for (int j = 0; j < n; j++)
    {
        B[j] = rand() % 10;
    }
}

void print_vector(float *B, int n)
{
    printf("OUTPUT: Вектор B: ");
    for (int i = 0; i < n; i++)
    {
        printf("%f ", B[i]);
    }
    printf("\n");
}

int main(int argc, char *argv[])
{
    int rank, size, m = 4, n = 4; // примерные размеры
    float *A = NULL;              // полная матрица
    float *local_A = NULL;        // локальная матрица
    float *B = NULL;              // вектор
    float *C = NULL;              // результат
    float *local_C = NULL;        // локальный результат

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Проверка на количество процессов
    if (size > m)
    {
        if (rank == 0)
        {
            printf("Ошибка: количество процессов не может превышать количество строк матрицы.\n");
        }
        MPI_Finalize();
        return 1;
    }

    // Инициализация генератора случайных чисел
    if (rank == 0)
    {
        srand(time(NULL));
        A = (float *)malloc(m * n * sizeof(float));
        B = (float *)malloc(n * sizeof(float));
        C = (float *)malloc(m * sizeof(float));
        initialize_matrix(A, m, n);
        initialize_vector(B, n);
        print_matrix(A, m, n);
        print_vector(B, n);
    }

    // Распределение векторов
    local_A = (float *)malloc(m / size * n * sizeof(float));
    MPI_Scatter(A, m / size * n, MPI_FLOAT, local_A, m / size * n, MPI_FLOAT, 0, MPI_COMM_WORLD);
    MPI_Bcast(B, n, MPI_FLOAT, 0, MPI_COMM_WORLD);

    // Локальное вычисление
    local_C = (float *)malloc(m / size * sizeof(float));
    for (int i = 0; i < m / size; i++)
    {
        local_C[i] = 0.0;
        for (int j = 0; j < n; j++)
        {
            local_C[i] += local_A[i * n + j] * B[j];
        }
    }

    // Сбор результатов
    MPI_Gather(local_C, m / size, MPI_FLOAT, C, m / size, MPI_FLOAT, 0, MPI_COMM_WORLD);

    // Вывод результатов
    if (rank == 0)
    {
        printf("OUTPUT: Результат умножения матрицы на вектор:\n");
        for (int i = 0; i < m; i++)
        {
            printf("OUTPUT: C[%d] = %f\n", i, C[i]);
        }
        free(A);
        free(B);
        free(C);
    }

    free(local_A);
    free(local_C);
    MPI_Finalize();
    return 0;
}
