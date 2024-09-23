COMPILE_CMD="mpicc -o test test.c -lm"

show_menu() {
    echo "Задание 1."
    echo "Задание 2."
}

read_choice() {
    local choice
    read -p "Введите номер программы (1-2): " choice

    case $choice in
    1)
        SOURCE_FILE="ring_shift_1.c"
        ;;
    2)
        SOURCE_FILE="ring_shift_2.c"
        ;;
    *)
        echo "Такого задания нет. Повторный выбор."
        read_choice
        ;;
    esac
}

show_menu
read_choice

# SOURCE_FILE="ring_force.c"

export COMPILE_CMD
export SOURCE_FILE
