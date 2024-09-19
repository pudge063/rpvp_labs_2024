#include <mpi.h>
#include <stdio.h>
#include <math.h>

// Заданная функция
double f(double x) {
    return pow(x, 3) / (0.5 * pow(x, 7) + x + 6);
}

// Численное интегрирование методом средних прямоугольников
double integrate(double a, double b, int n) {
    double h = (b - a) / n;
    double sum = 0.0;
    for (int i = 0; i < n; i++) {
        double x = a + h * (i + 0.5);
        sum += f(x);
    }
    return sum * h;
}

int main(int argc, char *argv[]) {
    int rank, size;
    double a = 0.4, b = 1.5;
    int n = 1000000;  // Начальное количество прямоугольников

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    double start_time, end_time;

    if (rank == 0) {
        start_time = MPI_Wtime();  // Запуск таймера
    }

    // Расчет участка интегрирования для каждого процесса
    double h = (b - a) / size;
    double local_a = a + rank * h;
    double local_b = local_a + h;

    double local_integral = integrate(local_a, local_b, n / size);

    // Сбор результатов от всех процессов
    double total_integral = 0.0;
    MPI_Reduce(&local_integral, &total_integral, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        end_time = MPI_Wtime();  // Конец таймера
        printf("OUTPUT: Значение интеграла: %lf\n", total_integral);
        printf("OUTPUT: Время выполнения: %lf секунд\n", end_time - start_time);
    }

    MPI_Finalize();
    return 0;
}
