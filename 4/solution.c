#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../method.h"

const int buf_size = 8192;

void sys_error(char* msg) {
    puts(msg);
    exit(-1);
}

int main(int argc, char* argv[]) {
    int fd[2], reading, writing;
    int buf_fd[2];
    ssize_t file_size;
    int file_d;
    ssize_t size;
    char str_buf[buf_size];

    int n;

    if (argc < 3) {
        printf("Usage: %s [in_file] [out_file]\n", argv[0]);
        exit(-1);
    }

    printf("Enter the lenth of string: ");
    scanf("%d", &n);

    if (set_len(n) > 0) {
        sys_error("Wrong length. Should be between 0 and 128");
    }

    if (pipe(fd) < 0) {
        sys_error("Can\'t open pipe\n");
    }

    if (pipe(buf_fd) < 0) {
        sys_error("Can\'t open pipe\n");
    }

    reading = fork();

    if (reading < 0) {
        sys_error("Can\'t fork child\n");
    } else if (reading > 0) {
        if (close(fd[0]) < 0) {
            sys_error("Can\'t close reading side of pipe\n");
        }

        if ((file_d = open(argv[1], O_RDONLY, 0666)) < 0) {
            sys_error("Can\'t open file\n");
        }

        if ((file_size = read(file_d, str_buf, buf_size)) < 0) {
            sys_error("Can\'t read string from file\n");
        }

        size = write(fd[1], str_buf, file_size);

        if (size != file_size) {
            sys_error("Can\'t write whole string to pipe\n");
        }

        if (close(fd[1]) < 0) {
            sys_error("Can\'t close writing side of pipe\n");
        }

    } else {
        if (close(fd[1]) < 0) {
            sys_error("Can\'t close writing side of pipe\n");
        }

        writing = fork();

        if (writing < 0) {
            sys_error("Can\'t fork child\n");
        } else if (writing > 0) {
            size = read(fd[0], str_buf, buf_size);

            if (size < 0) {
                sys_error("Can\'t read string from pipe\n");
            }

            char* ans_buf = (char*)malloc(size * sizeof(char));
            int len = calculate(str_buf, size, ans_buf);

            if (close(fd[0]) < 0) {
                sys_error("Can\'t close reading side of pipe\n");
            }

            if (len != n || len == 0) {
                ans_buf = "[no_answer]";
                len = 12;
            }

            printf("%d\n", len);

            size = write(buf_fd[1], ans_buf, len);

            free(ans_buf);

            if (size != len) {
                sys_error("Can\'t write whole string to pipe\n");
            }

            if (close(buf_fd[1]) < 0) {
                sys_error("Can\'t close writing side of pipe\n");
            }

        } else {
            if (close(buf_fd[1]) < 0) {
                sys_error("Can\'t close writing side of pipe\n");
            }

            size = read(buf_fd[0], str_buf, buf_size);

            if (size < 0) {
                sys_error("Can\'t read string from pipe\n");
            }

            if ((file_d = open(argv[2], O_WRONLY | O_TRUNC | O_CREAT, 0666)) < 0) {
                sys_error("Can\'t open file\n");
            }

            if (size >= 12 && strncmp(str_buf, "[no_answer]", 12) == 0) {
                size = 0;
            }

            size = write(file_d, str_buf, size);

            if (size < 0) {
                sys_error("Can\'t write in file\n");
            }

            if (close(file_d) < 0) {
                sys_error("Can\'t close writing file\n");
            }

            if (close(buf_fd[0]) < 0) {
                sys_error("Can\'t close reading side of pipe\n");
            }
        }
    }

    return 0;
}
/*
?───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
?─██████████████─████████──████████─██████████████─██████─────────██████████████─████████████████───██████████████─██████████████─██████████████─████████████████───
?─██░░░░░░░░░░██─██░░░░██──██░░░░██─██░░░░░░░░░░██─██░░██─────────██░░░░░░░░░░██─██░░░░░░░░░░░░██───██░░░░░░░░░░██─██░░░░░░░░░░██─██░░░░░░░░░░██─██░░░░░░░░░░░░██───
?─██░░██████░░██─████░░██──██░░████─██████████░░██─██░░██─────────██████████░░██─██░░████████░░██───██░░██████░░██─██████░░██████─██░░██████░░██─██░░████████░░██───
?─██░░██──██░░██───██░░░░██░░░░██───────────██░░██─██░░██─────────────────██░░██─██░░██────██░░██───██░░██──██░░██─────██░░██─────██░░██──██░░██─██░░██────██░░██───
?─██░░██████░░██───████░░░░░░████───██████████░░██─██░░██─────────██████████░░██─██░░████████░░██───██░░██████░░██─────██░░██─────██░░██──██░░██─██░░████████░░██───
?─██░░░░░░░░░░██─────██░░░░░░██─────██░░░░░░░░░░██─██░░██─────────██░░░░░░░░░░██─██░░░░░░░░░░░░██───██░░░░░░░░░░██─────██░░██─────██░░██──██░░██─██░░░░░░░░░░░░██───
?─██░░██████░░██───████░░░░░░████───██████████░░██─██░░██─────────██████████░░██─██░░██████░░████───██░░██████░░██─────██░░██─────██░░██──██░░██─██░░██████░░████───
?─██░░██──██░░██───██░░░░██░░░░██───────────██░░██─██░░██─────────────────██░░██─██░░██──██░░██─────██░░██──██░░██─────██░░██─────██░░██──██░░██─██░░██──██░░██─────
?─██░░██──██░░██─████░░██──██░░████─██████████░░██─██░░██████████─██████████░░██─██░░██──██░░██████─██░░██──██░░██─────██░░██─────██░░██████░░██─██░░██──██░░██████─
?─██░░██──██░░██─██░░░░██──██░░░░██─██░░░░░░░░░░██─██░░░░░░░░░░██─██░░░░░░░░░░██─██░░██──██░░░░░░██─██░░██──██░░██─────██░░██─────██░░░░░░░░░░██─██░░██──██░░░░░░██─
?─██████──██████─████████──████████─██████████████─██████████████─██████████████─██████──██████████─██████──██████─────██████─────██████████████─██████──██████████─
?───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
*/