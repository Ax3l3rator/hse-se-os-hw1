#include <stdlib.h>

#define ERR_WRONG_LENGTH 1

static unsigned int str_size;
static unsigned int cur_len = 0;
static unsigned char last_read_char = 128;

/*
 * Устанавливает значение длины искомой строки на num
 * @param num Значение на которое хотим установить
 * @return ERR_LENGTH_NEGATIVE если num < 0
 *  | 0, если num >= 0
 */
int set_len(int num) {
    if (num >= 0 && num <= 128) {
        str_size = num;
        return 0;
    }
    return ERR_WRONG_LENGTH;
}

/*
 * Функция поулчения длины строки
 * @return длину найденной строки
 */
int get_len() {
    return cur_len;
}

/*
 * Функция находящая строку длины str_size в которой значение каждого i-го ASCII-символа меньше, чем
 * (i-1)-го
 * @param str Строка по которой ведется поиск
 * @param len Длина str
 * @param answer Строка, в которую будет записываться ответ
 * @return Длина строки-ответа
 */
int calculate(char* str, int len, char* answer) {
    int counter = 0;
    for (int i = 0; i < len; ++i) {
        if (cur_len == str_size) {
            break;
        }
        if (str[i] < last_read_char) {
            if (cur_len == 0 && last_read_char != 128) {
                answer[counter++] = last_read_char;
                cur_len++;
            }
            answer[counter] = str[i];
            ++counter;

            ++cur_len;
        } else {
            counter = 0;
            cur_len = 0;
        }
        last_read_char = str[i];
    }

    return counter;
}