#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "../method.h"

const int buf_size = 8192;

void sys_error(char* msg) {
    puts(msg);
    exit(-1);
}

int main(int argc, char* argv[]) {
    int fd, process, buf_fd;
    ssize_t file_size;
    int file_d;
    ssize_t size;
    char str_buf[buf_size];

    char* name = "1.fifo";
    char* buf_name = "2.fifo";

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

    if (mkfifo(name, 0666)) {
        sys_error("Error creating FIFO");
    }

    if (mkfifo(buf_name, 0666)) {
        sys_error("Error creating FIFO");
    }

    process = fork();
    if (process < 0) {
        sys_error("Can\'t fork child\n");

    } else if (process > 0) {
        if ((fd = open(name, O_WRONLY)) < 0) {
            printf("Can\'t open FIFO for writing\n");
            exit(-1);
        }

        // * file section begin
        if ((file_d = open(argv[1], O_RDONLY, 0666)) < 0) {
            sys_error("Can\'t open file for reading\n");
        }

        if ((file_size = read(file_d, str_buf, buf_size)) < 0) {
            sys_error("Can\'t read string from file\n");
        }

        if (close(file_d) < 0) {
            sys_error("Can\'t close writing file\n");
        }
        // * file sction end

        // * fifo exchange begin
        size = write(fd, str_buf, file_size);

        if (close(fd) < 0) {
            sys_error("Can\'t close FIFO\n");
        }

        if (size != file_size) {
            sys_error("Can\'t write whole string to FIFO\n");
        }

        if ((buf_fd = open(buf_name, O_RDONLY)) < 0) {
            printf("Can\'t open FIFO for reading/writitng\n");
            exit(-1);
        }

        // printf("Boo");
        size = read(buf_fd, str_buf, buf_size);

        if (close(buf_fd) < 0) {
            sys_error("Can\'t close FIFO\n");
        }

        if (size < 0) {
            sys_error("Can\'t read string from FIFO\n");
        }
        // * fifo exchange end

        if ((file_d = open(argv[2], O_WRONLY | O_TRUNC | O_CREAT, 0666)) < 0) {
            sys_error("Can\'t open file\n");
        }

        size = write(file_d, str_buf, size);

        if (size < 0) {
            sys_error("Can\'t write in file\n");
        }

        if (close(file_d) < 0) {
            sys_error("Can\'t close writing file\n");
        }

        if (remove(name) < 0) {
            sys_error("cant remove FIFO");
        }

        if (remove(buf_name) < 0) {
            sys_error("cant remove FIFO");
        }
    } else {
        if ((fd = open(name, O_RDONLY)) < 0) {
            printf("Can\'t open FIFO for reading/writitng\n");
            exit(-1);
        }

        size = read(fd, str_buf, buf_size);

        if (size < 0) {
            sys_error("Can\'t read string from FIFO\n");
        }

        char* ans_buf = (char*)malloc(size * sizeof(char));
        int len = calculate(str_buf, size, ans_buf);
        if ((buf_fd = open(buf_name, O_WRONLY)) < 0) {
            printf("Can\'t open FIFO for reading/writitng\n");
            exit(-1);
        }

        if (len != n) {
            len = 0;
        }

        size = write(buf_fd, ans_buf, len);

        if (close(buf_fd) < 0) {
            sys_error("Can\'t close FIFO\n");
        }

        free(ans_buf);

        if (size != len) {
            sys_error("Can\'t write whole string to FIFO\n");
        }

        if (close(fd) < 0) {
            sys_error("Can\'t close FIFO\n");
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