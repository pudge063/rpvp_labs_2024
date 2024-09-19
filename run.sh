#!/bin/bash

set -a
[ -f .env ] && . .env
set +a

# LAB_N=$LAB_N

show_menu() {
    echo "Номер лабы:"
    echo "1) Лабораторная 1. Основные схемы обмена сообщениями"
    echo "2) Лабораторная 2. Параллельное интегрирование"
    echo "3) Лабораторная 3. Параллельное интегрирование"
    echo "4) Лабораторная 4. Параллельное интегрирование"
    echo "5) Лабораторная 5. Параллельное интегрирование"
    echo "6) Лабораторная 6. Параллельное интегрирование"

}

read_choice() {
    local choice
    read -p "Введите номер программы (1-4): " choice

    case $choice in
    1)
        LAB_N=1
        ;;
    2)
        LAB_N=2
        ;;
    3)
        LAB_N=3
        ;;
    4)
        LAB_N=4
        ;;
    5)
        LAB_N=5
        ;;
    6)
        LAB_N=6
        ;;
    *)
        echo "Неверный выбор. Попробуйте снова."
        read_choice
        ;;
    esac
}

show_menu
read_choice

source runs/lab_$LAB_N.sh

LOG_FILE="logs/lab_$LAB_N/logs_$SOURCE_FILE.txt"
DOCKER_COMPOSE_FILE="docker-compose.yml"

>"$LOG_FILE"
echo $(date -u) >>"$LOG_FILE"

run_container() {
    local service_name=$1
    echo "Пересборка и запуск сервиса: $service_name" | tee -a "$LOG_FILE"

    export SOURCE_FILE
    export LAB_N
    export COMPILE_CMD
    export VARIANT

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
