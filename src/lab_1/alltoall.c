#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define COUNT 1024

// реализация all-to-all
// каждый процесс передаёт сообщения всем процессам

int main(int argc, char *argv[]) {
    int size, rank;
    char send_buf[COUNT], recv_buf[COUNT];
    MPI_Request *send_requests, *recv_requests;
    MPI_Status *statuses;
    double start_time, end_time;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    for (int i = 0; i < COUNT; i++) {
        send_buf[i] = rank;
    }

    send_requests = (MPI_Request*)malloc(size * sizeof(MPI_Request)); // выделение памяти для запросов
    recv_requests = (MPI_Request*)malloc(size * sizeof(MPI_Request));
    statuses = (MPI_Status*)malloc(size * sizeof(MPI_Status)); // выдделение памяти для статусов

    start_time = MPI_Wtime(); // таймер старт

    for (int i = 0; i < size; i++) {
        MPI_Isend(send_buf, COUNT, MPI_CHAR, i, 0, MPI_COMM_WORLD, &send_requests[i]); // оправляет данные из буфера отправки процессу i
        MPI_Irecv(recv_buf, COUNT, MPI_CHAR, i, 0, MPI_COMM_WORLD, &recv_requests[i]); // получает в recive буфер данные от процесса i
        // операции не блокирующие (процессы продолжают выполнение, не дожидаясь завершения операций)
    }

    MPI_Waitall(size, send_requests, statuses); 
    MPI_Waitall(size, recv_requests, statuses); // ожидание завершения всех процессов

    end_time = MPI_Wtime(); // запись таймера

    if (rank == 0) {
        printf("OUTPUT: Время для all-to-all передачи на %d процессах: %f секунд\n", size, end_time - start_time);
    }

    // free(send_requests);
    // free(recv_requests);
    // free(statuses);

    MPI_Finalize(); // выход MPI

    return 0;
}
