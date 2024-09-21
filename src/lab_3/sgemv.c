#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define ROWS 8
#define COLS 8

void initialize_matrix(float *matrix, int rows, int cols);
void initialize_vector(float *vector, int size);
void print_vector(float *vector, int size);

int main(int argc, char *argv[])
{
    int rank, size;
    float *matrix = NULL, *local_matrix = NULL;
    float *vector = NULL, *result = NULL;
    float *local_result = NULL;
    int rows = ROWS, cols = COLS;
    int local_rows;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    local_rows = rows / size; // Предполагается, что rows делится на size без остатка
    if (rows % size != 0) {
        if (rank == 0) {
            fprintf(stderr, "Error: rows %d is not divisible by size %d\n", rows, size);
        }
        MPI_Finalize();
        exit(EXIT_FAILURE);
    }

    if (rank == 0)
    {
        matrix = (float *)malloc(rows * cols * sizeof(float));
        vector = (float *)malloc(cols * sizeof(float));
        result = (float *)malloc(rows * sizeof(float));

        initialize_matrix(matrix, rows, cols);
        initialize_vector(vector, cols);

        // Распределяем часть матрицы по процессам
        for (int i = 1; i < size; i++)
        {
            MPI_Send(matrix + i * local_rows * cols, local_rows * cols, MPI_FLOAT, i, 0, MPI_COMM_WORLD);
        }
    }
    else
    {
        local_matrix = (float *)malloc(local_rows * cols * sizeof(float));
        vector = (float *)malloc(cols * sizeof(float));
        local_result = (float *)malloc(local_rows * sizeof(float));

        MPI_Recv(local_matrix, local_rows * cols, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, &status);
    }

    // Распределяем вектор по всем процессам
    MPI_Bcast(vector, cols, MPI_FLOAT, 0, MPI_COMM_WORLD);

    // Умножаем локальную часть матрицы на вектор
    if (rank == 0)
    {
        local_matrix = matrix;
    }
    else
    {
        free(matrix); // Освобождаем память на процессе 0
    }

    if (rank != 0) {
        local_result = (float *)malloc(local_rows * sizeof(float));
    }

    for (int i = 0; i < local_rows; i++)
    {
        local_result[i] = 0.0f;
        for (int j = 0; j < cols; j++)
        {
            local_result[i] += local_matrix[i * cols + j] * vector[j];
        }
    }

    // Собираем результаты
    if (rank == 0)
    {
        result = (float *)malloc(rows * sizeof(float));
    }

    MPI_Gather(local_result, local_rows, MPI_FLOAT, result, local_rows, MPI_FLOAT, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        printf("OUTPUT: Result Vector:\n");
        print_vector(result, rows);
        free(matrix);
        free(vector);
        free(result);
    }
    else
    {
        free(local_matrix);
        free(local_result);
        free(vector);
    }

    MPI_Finalize();
    return 0;
}

void initialize_matrix(float *matrix, int rows, int cols)
{
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            matrix[i * cols + j] = (float)(i + j);
        }
    }
}

void initialize_vector(float *vector, int size)
{
    for (int i = 0; i < size; i++)
    {
        vector[i] = (float)i;
    }
}

void print_vector(float *vector, int size)
{
    for (int i = 0; i < size; i++)
    {
        printf("%f ", vector[i]);
    }
    printf("\n");
}
