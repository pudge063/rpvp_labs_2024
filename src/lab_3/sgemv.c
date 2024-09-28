#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define N 64  // Размер матрицы

// Функция для умножения части матрицы на вектор
void matrix_vector_multiply(float* local_A, float* B, float* local_C, int local_rows) {
    for (int i = 0; i < local_rows; i++) {
        local_C[i] = 0.0;
        for (int j = 0; j < N; j++) {
            local_C[i] += local_A[i * N + j] * B[j];
        }
    }
}

int main(int argc, char** argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int local_rows = N / size;  // Количество строк, обрабатываемых каждым процессом
    float A[N * N];  // Исходная матрица
    float B[N];      // Вектор
    float C[N];      // Результирующий вектор
    float* local_A = (float*)malloc(local_rows * N * sizeof(float));
    float* local_C = (float*)malloc(local_rows * sizeof(float));

    // Инициализация матрицы и вектора только в процессе с rank 0
    if (rank == 0) {
        for (int i = 0; i < N * N; i++) {
            A[i] = i + 1;  // Пример значений для матрицы
        }
        for (int i = 0; i < N; i++) {
            B[i] = i + 1;  // Пример значений для вектора
        }
    }

    // Разделяем матрицу по процессам
    MPI_Scatter(A, local_rows * N, MPI_FLOAT, local_A, local_rows * N, MPI_FLOAT, 0, MPI_COMM_WORLD);

    // Распространяем вектор B по всем процессам
    MPI_Bcast(B, N, MPI_FLOAT, 0, MPI_COMM_WORLD);

    // Локальное умножение части матрицы на вектор
    matrix_vector_multiply(local_A, B, local_C, local_rows);

    // Собираем результат в процесс 0
    MPI_Gather(local_C, local_rows, MPI_FLOAT, C, local_rows, MPI_FLOAT, 0, MPI_COMM_WORLD);

    // Процесс с rank 0 выводит результат
    if (rank == 0) {
        printf("OUTPUT: Результат умножения матрицы на вектор:\n");
        for (int i = 0; i < N; i++) {
            printf("OUTPUT: C[%d] = %f\n", i, C[i]);
        }
    }

    free(local_A);
    free(local_C);
    MPI_Finalize();
    return 0;
}
