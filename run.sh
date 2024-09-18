#!/bin/bash

set -a
[ -f .env ] && . .env
set +a

show_menu() {
    echo "Выберите программу:"
    echo "1) ring.c"
    echo "2) broadcast.c"
    echo "3) gather.c"
    echo "4) alltoall.c"
}

read_choice() {
    local choice
    read -p "Введите номер программы (1-4): " choice

    case $choice in
    1)
        SOURCE_FILE="ring.c"
        ;;
    2)
        SOURCE_FILE="broadcast.c"
        ;;
    3)
        SOURCE_FILE="gather.c"
        ;;
    4)
        SOURCE_FILE="alltoall.c"
        ;;
    *)
        echo "Неверный выбор. Попробуйте снова."
        read_choice
        ;;
    esac
}

show_menu
read_choice

LOG_FILE="logs_$SOURCE_FILE.txt"
DOCKER_COMPOSE_FILE="docker-compose.yml"

>"$LOG_FILE"
echo $(date -u) >>"$LOG_FILE"

run_container() {
    local service_name=$1
    echo "Пересборка и запуск сервиса: $service_name" | tee -a "$LOG_FILE"

    export SOURCE_FILE

    docker-compose build "$service_name" | grep "OUTPUT:" | tee -a "$LOG_FILE"

    docker-compose up "$service_name" | grep "OUTPUT:" | tee -a "$LOG_FILE"

    docker-compose down | tee -a "$LOG_FILE"

    echo -e "\n" | tee -a "$LOG_FILE"
}

echo "Запуск с ${MPI_CONTAINER_1} процессами:" | tee -a "$LOG_FILE"
run_container mpi-app-1

echo "Запуск с ${MPI_CONTAINER_2} процессами:" | tee -a "$LOG_FILE"
run_container mpi-app-2

echo "Запуск с ${MPI_CONTAINER_3} процессами:" | tee -a "$LOG_FILE"
run_container mpi-app-3

echo "Все контейнеры выполнены." | tee -a "$LOG_FILE"
