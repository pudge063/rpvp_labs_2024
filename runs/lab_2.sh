show_menu() {
    echo "Номер задания:"
    echo "1) Параллельное численное интегрирование методом средних прямоугольников"
    echo "2) Численное параллельное интегрирование методом Монте-Карло"
}

read_choice() {
    local choice
    read -p "Введите номер программы (1-2): " choice

    case $choice in
    1)
        SOURCE_FILE="middle_rectangle.c"
        ;;
    2)
        SOURCE_FILE="monte_carlo.c"
        ;;
    *)
        echo "Неверный выбор. Попробуйте снова."
        read_choice
        ;;
    esac
}

show_menu
read_choice

COMPILE_CMD="mpicc -o test test.c -lm"

export SOURCE_FILE
export COMPILE_CMD
