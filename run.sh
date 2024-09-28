#!/bin/bash

set -a
[ -f .env ] && . .env
set +a

echo "Текущий вариант $VARIANT. Изменить вариант в .env файле."

show_menu() {
    echo "Номер лабы:"
    echo "1) Лабораторная 1. Основные схемы обмена сообщениями"
    echo "2) Лабораторная 2. Параллельное интегрирование"
    echo "3) Лабораторная 3. Параллельное умножение матрицы на вектор"
    echo "4) Лабораторная 4. Обмен данными на системе компьютеров с топологией кольцо."
    echo "5) Лабораторная 5. Обмен данными на системе компьютеров с топологией линейка."
    echo "6) Лабораторная 6. Произведение двух матриц в топологии кольцо."
}

read_choice() {
    local choice
    read -p "Введите номер программы (1-6): " choice

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
        echo "Такой лабы нет. Повторный ввод."
        read_choice
        ;;
    esac
}

show_menu
read_choice
LANG="c"
source runs/lab_$LAB_N.sh

LOG_FILE="logs/lab_$LAB_N/logs_$SOURCE_FILE.txt"
DOCKER_COMPOSE_FILE="docker-compose.yml"
DEBUG_LOG_FILE="logs/debug.txt"

>"$LOG_FILE"
>"$DEBUG_LOG_FILE"
echo $(date -u) >>"$LOG_FILE"
echo -e "\n" | tee -a "$LOG_FILE"

run_container() {
    local service_name=$1
    echo "Пересборка и запуск сервиса: $service_name" | tee -a "$LOG_FILE"

    export SOURCE_FILE
    export LAB_N
    export COMPILE_CMD
    export VARIANT
    export MATRIX_ARGS
    export LANG

    docker-compose build "$service_name" | tee -a "$DEBUG_LOG_FILE"

    docker-compose up "$service_name" | tee -a "$DEBUG_LOG_FILE"

    docker-compose logs "$service_name" | grep -E "OUTPUT:" | tee -a "$LOG_FILE"

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
