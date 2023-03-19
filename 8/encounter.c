#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

const int buffer_size = 8192;

void sys_error(char* msg) {
    puts(msg);
    exit(-1);
}

int main(int argc, char* argv[]) {
    int fifo_write_descriptor, fifo_read_descriptor;
    int file_read_descriptor, file_write_descriptor;
    ssize_t file_read_size, file_write_size;
    ssize_t fifo_write_size, fifo_read_size;
    char buffer[buffer_size];

    char* fifo_write_name = "1.fifo";
    char* fifo_read_name = "2.fifo";

    if (argc < 3) {
        printf("Usage: %s [in_file] [out_file]\n", argv[0]);
        exit(-1);
    }

    (void)umask(0);

    if (mkfifo(fifo_write_name, 0666)) {
        sys_error("Error creating FIFO");
    }

    if (mkfifo(fifo_read_name, 0666)) {
        sys_error("Error creating FIFO");
    }

    // * file section begin
    if ((file_read_descriptor = open(argv[1], O_RDONLY)) < 0) {
        sys_error("Can\'t open file for reading\n");
    }

    if ((file_write_descriptor = open(argv[2], O_WRONLY | O_TRUNC | O_CREAT)) < 0) {
        sys_error("Can\'t open file\n");
    }

    if ((fifo_write_descriptor = open(fifo_write_name, O_WRONLY)) < 0) {
        printf("Can\'t open FIFO for writing\n");
        exit(-1);
    }

    if ((fifo_read_descriptor = open(fifo_read_name, O_RDONLY)) < 0) {
        printf("Can\'t open FIFO for reading/writitng\n");
        exit(-1);
    }

    char answer[buffer_size];

    if ((file_read_size = read(file_read_descriptor, buffer, buffer_size)) < 0) {
        sys_error("Can\'t read from file");
    }

    fifo_write_size = write(fifo_write_descriptor, buffer, file_read_size);

    if (fifo_write_size != file_read_size) {
        sys_error("Can\'t write whole string to FIFO\n");
    }

    if ((fifo_read_size = read(fifo_read_descriptor, answer, buffer_size)) < 0) {
        sys_error("Can\'t read fifo\n");
    }

    if (file_write_size = write(file_write_descriptor, answer, fifo_read_size) < 0) {
        sys_error("Can\'t write to file");
    }

    if (fifo_read_size >= 12 && strncmp(answer, "[no_answer]", 12) == 0) {
        printf("boo");
        truncate(argv[2], 0);
    }

    if (close(fifo_write_descriptor) < 0) {
        sys_error("Can\'t close FIFO\n");
    }

    if (remove(fifo_write_name) < 0) {
        sys_error("cant remove FIFO");
    }

    if (close(file_read_descriptor) < 0) {
        sys_error("Can\'t close reading file\n");
    }

    if (close(fifo_read_descriptor) < 0) {
        sys_error("Can\'t close FIFO\n");
    }

    if (close(file_write_descriptor) < 0) {
        sys_error("Can\'t close writing file\n");
    }

    if (remove(fifo_read_name) < 0) {
        sys_error("cant remove FIFO");
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