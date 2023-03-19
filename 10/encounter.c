#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define msg_buf 200

void sys_error(char* msg) {
    puts(msg);
    exit(-1);
}

typedef struct msgbuf {
    long mtype;
    char mtext[msg_buf];
} message_buf;

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Usage: %s [in_file] [out_file]\n", argv[0]);
        exit(-1);
    }

    int file_read_descriptor, file_write_descriptor;
    ssize_t file_read_size, file_write_size;

    int firsd_qid, second_qid;

    key_t first_key = 10, second_key = 20;

    message_buf* buf = (message_buf*)malloc(sizeof(message_buf));

    buf->mtype = 1;
    if ((firsd_qid = msgget(first_key, IPC_CREAT | 0666)) < 0) {
        sys_error("Can\'t open msg queue");
    }

    if ((second_qid = msgget(second_key, IPC_CREAT | 0666)) < 0) {
        sys_error("Can\'t open msg queue");
    }

    if ((file_read_descriptor = open(argv[1], O_RDONLY)) < 0) {
        sys_error("Can\'t open file for reading\n");
    }

    if ((file_write_descriptor = open(argv[2], O_WRONLY | O_CREAT | O_APPEND)) < 0) {
        sys_error("Can\'t open file for reading\n");
    }

    truncate(argv[2], 0);

    ssize_t q_r_size;

    while ((file_read_size = read(file_read_descriptor, buf->mtext, msg_buf)) > 0) {
        if (msgsnd(firsd_qid, buf, file_read_size, 0) < 0) {
            sys_error("Can\'t write message");
        }

        if ((q_r_size = msgrcv(second_qid, buf, msg_buf, 1, 0)) < 0) {
            sys_error("Can\'t read message");
        }

        if (write(file_write_descriptor, buf->mtext, q_r_size) < 0) {
            sys_error("Can\'t write to file");
        }

        if (q_r_size >= 12 && strncmp(buf->mtext, "[no_answer]", 12) == 0) {
            truncate(argv[2], 0);
        }
    }

    if (msgctl(firsd_qid, IPC_RMID, NULL) < 0) {
        sys_error("Can\'t clean queue");
    }

    if ((q_r_size = msgrcv(second_qid, buf, msg_buf, 1, 0)) < 0) {
        sys_error("Can\'t read message");
    }

    if (q_r_size >= 12 && strncmp(buf->mtext, "[no_answer]", 12) == 0) {
        truncate(argv[2], 0);
    }

    if (msgctl(second_qid, IPC_RMID, NULL) < 0) {
        sys_error("Can\'t clean queue");
    }

    free(buf);
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