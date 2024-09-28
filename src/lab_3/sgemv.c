#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define MEMORY_LIMIT (8ULL * 1024 * 1024 * 1024) // 8 ГБ в байтах (используем 8ULL для обозначения беззнакового длинного целого)

size_t calculate_max_n(size_t memory_limit, size_t num_processes)
{
    size_t size_of_float = sizeof(float);
    size_t adjusted_memory_limit = memory_limit / num_processes; // делим память на количество процессов

    size_t N = 1;
    while (N * (N + 1) * size_of_float <= adjusted_memory_limit)
    {
        N++;
    }

    return N - 1; // возвращаем максимальный размер N
}

// функция для умножения части матрицы на вектор
void matrix_vector_multiply(float *local_A, float *B, float *local_C, int local_rows, size_t N)
{
    for (int i = 0; i < local_rows; i++)
    {
        local_C[i] = 0.0;
        for (size_t j = 0; j < N; j++)
        { // изменено на size_t для N
            local_C[i] += local_A[i * N + j] * B[j];
        }
    }
}

int main(int argc, char **argv)
{
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // рассчитываем максимальный размер N в зависимости от количества процессов
    size_t max_n = calculate_max_n(MEMORY_LIMIT, size);
    if (rank == 0)
    {
        printf("OUTPUT: Для %d процессов: Максимальный размер матрицы N = %zu\n", size, max_n);
    }

    // обновляем размер матрицы на основе вычисленного максимального N

    int N = 15000; // размер матрицы

    int local_rows = N / size;                         // Количество строк, обрабатываемых каждым процессом
    float *A = (float *)malloc(N * N * sizeof(float)); // Исходная матрица
    float *B = (float *)malloc(N * sizeof(float));     // Вектор
    float *C = (float *)malloc(N * sizeof(float));     // Результирующий вектор
    float *local_A = (float *)malloc(local_rows * N * sizeof(float));
    float *local_C = (float *)malloc(local_rows * sizeof(float));

    // инициализация матрицы и вектора только в процессе с rank 0
    if (rank == 0)
    {
        for (int i = 0; i < N * N; i++)
        {
            A[i] = i + 1;
        }
        for (int i = 0; i < N; i++)
        {
            B[i] = i + 1;
        }
    }

    // разделяем матрицу по процессам
    MPI_Scatter(
        A,               // атрибуты А -  буфер отправки (исх. матрица)
        local_rows * N,  // количество элементов
        MPI_FLOAT,       // типп данных
        local_A,         // буфер получения
        local_rows * N,  // recv count - сколько получит каждый процесс
        MPI_FLOAT,       // тип данных
        0,               // ранг процесса отправителя (root)
        MPI_COMM_WORLD); // коммуникатор

    // распространяем вектор B по всем процессам
    MPI_Bcast(
        B, // буфер отправки
        N, // количество элементов
        MPI_FLOAT, // тип данных
        0, // ранг процесса транслятор
        MPI_COMM_WORLD); // коммуникатор

    double start_time = MPI_Wtime(); // начало замера времени

    // локальное умножение части матрицы на вектор
    matrix_vector_multiply(local_A, B, local_C, local_rows, N);

    // собираем результат в процесс 0 (отправитель, получатель, ранг получателя)
    MPI_Gather(local_C, local_rows, MPI_FLOAT, C, local_rows, MPI_FLOAT, 0, MPI_COMM_WORLD);

    // все данные собираем на процесс 0

    // завершаем замер времени
    double end_time = MPI_Wtime();

    // процесс с rank 0 выводит результат и время выполнения
    if (rank == 0)
    {
        // printf("OUTPUT: Результат умножения матрицы на вектор:\n");
        for (int i = 0; i < N; i++)
        {
            // printf("OUTPUT: C[%d] = %f\n", i, C[i]);
        }

        double serial_time = end_time - start_time;

        printf("OUTPUT: Время выполнения на %d процессах: S(%d) = %f\n", size, size, serial_time);
    }

    free(A);
    free(B);
    free(C);
    free(local_A);
    free(local_C);
    MPI_Finalize();
    return 0;
}
