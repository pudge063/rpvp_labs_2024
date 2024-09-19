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

COMPILE_CMD='mpicc -o test test.c'

export SOURCE_FILE
export COMPILE_CMD