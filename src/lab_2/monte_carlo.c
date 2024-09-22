#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

double f1(double x, double y)
{
    return x / pow(y, 2);
}

double f2(double x, double y)
{
    return pow(exp(x + y), 2);
}

double f3(double x, double y)
{
    return pow(exp(x - y), 2);
}

// генерация случайного числа в диапазоне [min, max]
double rand_double(double min, double max)
{
    return min + (max - min) * ((double)rand() / RAND_MAX);
}

// вычисление интеграла методом monte-carlo

/*
    вычисление интеграла методом выбора случайных чисел для оценки значений интеграла
    случайным образом генерируются точки в заданной области и используются для оценки средней величины функции * S 
*/

double monte_carlo_integral(int num_points, int rank, int size, int variant)
{
    int local_points = num_points / size;
    double local_sum = 0.0;

    for (int i = 0; i < local_points; i++)
    {
        double x, y;
        switch (variant)
        {
        case 1:
            x = rand_double(0.0, 1.0);
            y = rand_double(2.0, 5.0);
            local_sum += f1(x, y);
            break;
        case 2:
            x = rand_double(0.0, 1.0);
            y = rand_double(0.0, 1.0 - x);
            local_sum += f2(x, y);
            break;
        case 3:
            x = rand_double(-1.0, 0.0);
            y = rand_double(0.0, 1.0);
            local_sum += f3(x, y);
            break;
        default:
            printf("Неверный вариант задания!\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
    }

    return local_sum * (1.0 * 1.0) / local_points;
}

int main(int argc, char *argv[])
{
    int rank, size;
    int variants[] = {10000000, 100000000};
    double global_results[2] = {0.0, 0.0};

    char *env_variant = getenv("VARIANT");
    int variant = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (env_variant != NULL)
    {
        variant = atoi(env_variant) % 3 + 1; // конвертация в строку и вычисление варианта по номеру в списке
        if (rank == 0)
        {
            printf("OUTPUT: Вариант: %d\n", variant);
        }
    }

    srand(time(NULL) + rank);

    for (int i = 0; i < 2; i++)
    {
        int n = pow(10, variants[i]); // выбор n

        double start_time = MPI_Wtime(); // запуск таймера

        double local_result = monte_carlo_integral(n, rank, size, variant); // вычисление локальных интегралов на разных процессах

        MPI_Reduce(&local_result, &global_results[i], 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD); // сложение результатов всех процессов на 0

        double end_time = MPI_Wtime(); // стоп таймера

        if (rank == 0)
        {
            printf("OUTPUT: Значение интеграла для n = %d: %lf\n", variants[i], global_results[i]);
            printf("OUTPUT: Время выполнения для n = %d: %lf секунд\n", variants[i], end_time - start_time);
        }
    }

    MPI_Finalize();
    return 0;
}
