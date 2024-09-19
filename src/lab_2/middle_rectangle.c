#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

double f_variant_1(double x)
{
    return (1 - exp(0.7 / x)) / (2 + x);
}

double f_variant_2(double x)
{
    return log(1 + x) / x;
}

double f_variant_3(double x)
{
    return sqrt(x * (3 - x)) / (x + 1);
}

double f_variant_4(double x)
{
    return sin(x + 2) / (0.4 + cos(x));
}

double f_variant_5(double x)
{
    return x / pow(sin(2 * x), 3);
}

double f_variant_6(double x)
{
    return pow(x, 4) / (0.5 * pow(x, 2) + x + 6);
}

// метод вычисления интеграла прямоугольниками
double integrate(double (*f)(double), double a, double b, int n)
{
    double h = (b - a) / n;
    double sum = 0.0;
    for (int i = 0; i < n; i++)
    {
        double x = a + h * (i + 0.5);
        sum += f(x);
    }
    return sum * h;
}

int main(int argc, char *argv[])
{
    int rank, size;
    int n = 1000000; // начальное количество прямоугольников

    // init MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    char *env_variant = getenv("VARIANT");
    int variant = 0;

    if (env_variant != NULL)
    {
        variant = atoi(env_variant) % 6 + 1; // выбор варианта
        if (rank == 0)
        {
            printf("OUTPUT: Variant: %d\n", variant);
        }
    }
    else
    {
        if (rank == 0)
        {
            printf("OUTPUT: Переменная VARIANT не установлена.\n");
        }
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    // границы интеграла
    double (*f)(double) = NULL;
    double a = 0.0, b = 1.0;

    switch (variant)
    {
    case 1:
        f = f_variant_1;
        a = 1.0;
        b = 2.0;
        break;
    case 2:
        f = f_variant_2;
        a = 0.1;
        b = 1.0;
        break;
    case 3:
        f = f_variant_3;
        a = 1.0;
        b = 1.2;
        break;
    case 4:
        f = f_variant_4;
        a = -1.0;
        b = 1.0;
        break;
    case 5:
        f = f_variant_5;
        a = 0.1;
        b = 0.5;
        break;
    case 6:
        f = f_variant_6;
        a = 0.4;
        b = 1.5;
        break;
    default:
        if (rank == 0)
        {
            printf("OUTPUT: Неверный вариант задания.\n");
        }
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    double start_time, end_time;

    // старт таймера
    if (rank == 0)
    {
        start_time = MPI_Wtime();
    }

    // участок интегрирования каждого процесса
    double h = (b - a) / size;
    double local_a = a + rank * h;
    double local_b = local_a + h;

    // выполнение интегрирования каждого участка
    double local_integral = integrate(f, local_a, local_b, n / size);

    // сбор интегралов с каждого процесса на процесс 0
    double total_integral = 0.0;
    MPI_Reduce(&local_integral, &total_integral, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    // лог результата
    if (rank == 0)
    {
        end_time = MPI_Wtime();
        printf("OUTPUT: Значение интеграла: %lf\n", total_integral);
        printf("OUTPUT: Время выполнения: %lf секунд\n", end_time - start_time);
    }

    MPI_Finalize();
    return 0;
}
