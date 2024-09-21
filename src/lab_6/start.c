#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

void read_matrix(const char *filename, double **matrix, int rows, int cols)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        fprintf(stderr, "OUTPUT: Error opening file: %s\n", filename);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            if (fscanf(file, "%lf", &matrix[i][j]) != 1)
            {
                fprintf(stderr, "OUTPUT: Error reading matrix data from file: %s\n", filename);
                MPI_Abort(MPI_COMM_WORLD, 1);
            }
        }
    }
    fclose(file);
}

void write_matrix(const char *filename, double **matrix, int rows, int cols)
{
    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        fprintf(stderr, "OUTPUT: Error opening file for writing: %s\n", filename);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            fprintf(file, "%.2f ", matrix[i][j]);
        }
        fprintf(file, "\n");
    }
    fclose(file);
}

int main(int argc, char **argv)
{
    int rank, size;
    int n1, n2, n3;

    // Инициализация MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Проверка аргументов
    if (argc != 4)
    {
        if (rank == 0)
        {
            printf("OUTPUT: Usage: %s <n1> <n2> <n3>\n", argv[0]);
        }
        MPI_Finalize();
        return 1;
    }

    n1 = atoi(argv[1]);
    n2 = atoi(argv[2]);
    n3 = atoi(argv[3]);

    // Расчет строк для процесса
    int rows_per_process = n1 / size;
    int remaining_rows = n1 % size;
    int rows_for_this_process = (rank < remaining_rows) ? rows_per_process + 1 : rows_per_process;

    // Выделение памяти
    double **A = malloc(rows_for_this_process * sizeof(double *));
    double **B = malloc(n2 * sizeof(double *));
    double **C = malloc(rows_for_process * sizeof(double *));

    for (int i = 0; i < rows_for_this_process; i++)
    {
        A[i] = malloc(n2 * sizeof(double));
    }
    for (int i = 0; i < n2; i++)
    {
        B[i] = malloc(n3 * sizeof(double));
    }
    for (int i = 0; i < rows_per_process; i++)
    {
        C[i] = malloc(n3 * sizeof(double));
    }

    // Чтение матриц
    char filename[50];
    sprintf(filename, "A_%d.txt", rank);
    read_matrix(filename, A, rows_for_this_process, n2);
    sprintf(filename, "B_%d.txt", rank);
    read_matrix(filename, B, n2, n3);

    // Вычисление произведения матриц
    for (int i = 0; i < rows_for_this_process; i++)
    {
        for (int j = 0; j < n3; j++)
        {
            C[i][j] = 0;
            for (int k = 0; k < n2; k++)
            {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }

    // Обмен результатами
    for (int step = 0; step < size; step++)
    {
        MPI_Sendrecv_replace(B, n2 * n3, MPI_DOUBLE, (rank + 1) % size, 0,
                             (rank - 1 + size) % size, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        for (int i = 0; i < rows_for_this_process; i++)
        {
            for (int j = 0; j < n3; j++)
            {
                C[i][j] = 0;
                for (int k = 0; k < n2; k++)
                {
                    C[i][j] += A[i][k] * B[k][j];
                }
            }
        }
    }

    // Запись результата только для нулевого процесса
    if (rank == 0)
    {
        write_matrix("C.txt", C, n1, n3);
    }

    // Освобождение памяти
    for (int i = 0; i < rows_for_this_process; i++)
    {
        free(A[i]);
    }
    free(A);

    for (int i = 0; i < n2; i++)
    {
        free(B[i]);
    }
    free(B);

    for (int i = 0; i < rows_per_process; i++)
    {
        free(C[i]);
    }
    free(C);

    // Завершение MPI
    MPI_Finalize();
    return 0;
}
