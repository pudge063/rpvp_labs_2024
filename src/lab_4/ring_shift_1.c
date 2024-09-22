#include <mpi.h>
#include <stdio.h>
#define NUM_DIMS 1

int main(int argc, char **argv)
{

    /*
    реализация топологии "кольцо" в которой каждый процесс может обмениваться сообщениями с двумя соседями

    программа передает данные вдоль кольца и выводит информацию о каждом процессе



    каждый процесс передает данные только своему соседу с большим рангом

    все процессы могут отправлять и получать данные одновременно

    */

    int rank, size, i, A, B;
    int dims[NUM_DIMS];                  // размерности топологии, здесь = 1 (dims - одномерный массив)
    int periods[NUM_DIMS], source, dest; // определение будет ли топология периодичной, здесь = 1 (топология кольцо)
    // source и dest - ранг процесса инициатора и получателя сообщения
    int reorder = 0;    // запрещает менять ранги процессов для оптимизации, чтобы сохранить начальный порядок рангов
    MPI_Comm comm_cart; // определение коммуникатора
    MPI_Status status;  // хранение статуса операции отправки и получения message

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); //
    MPI_Comm_size(MPI_COMM_WORLD, &size); //

    A = rank; // текущий процесс
    B = -1;   // хранение полученных данных от соседнего процесса

    for (i = 0; i < NUM_DIMS; i++) // init dims, periosd
    {
        dims[i] = 0;    // количество процессов будет определено позже через MPI_Dims_create
        periods[i] = 1; // указание на периодичность топологии
    }

    MPI_Dims_create(size, NUM_DIMS, dims); // заполнение dims для равномерного распределения процессов, size - количество процессов
    // NM_DIMS - размерность (здесь = 1). -> если процесса 4, то здесь будет 4  процесса в одномерной топологии

    // создание новоый одноразмерной кольцевой топологии comm_cart
    MPI_Cart_create(MPI_COMM_WORLD, NUM_DIMS, dims, periods, reorder, &comm_cart);

    MPI_Cart_shift(comm_cart, 0, 1, &source, &dest); // функция для определения соседей в новой топологии

    MPI_Sendrecv(&A, 1, MPI_INT, dest, 0, &B, 1, MPI_INT, source, 0, comm_cart, &status); // передача данных по кольцу

    // вывод информации о ранге и полученном значении B
    printf("OUTPUT: rank=%d B=%d\n", rank, B);

    // Освобождаем ресурсы
    MPI_Comm_free(&comm_cart);
    MPI_Finalize();
    return 0;
}
