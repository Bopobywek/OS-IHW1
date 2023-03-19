#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "constants.h"
#include <sys/wait.h>
#include <semaphore.h>
#include <sys/msg.h>

void continueIfFileCouldBeOpened(char *filename, int status) {
    if (status < 0) {
        printf("Can't open file %s\n", filename);
        exit(-1);
    }
}

typedef struct {
    long mtype;
    char mtext[MESSAGE_SIZE];
} message_t;

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Invalid args\n");
        exit(-1);
    }

    int msqid = msgget(MSG_KEY, IPC_CREAT | 0666);
    if (msqid < 0) {
        printf("Can't create message queue\n");
        exit(-1);
    }

    char *filename_in = argv[1];
    char *filename_out = argv[2];

    int fd_in = open(filename_in, O_RDONLY);
    int fd_out = open(filename_out, O_WRONLY | O_CREAT | O_TRUNC, 0666);

    continueIfFileCouldBeOpened(filename_in, fd_in);
    continueIfFileCouldBeOpened(filename_out, fd_out);

    message_t message;
    message.mtype = 1;
    ssize_t read_bytes = read(fd_in, message.mtext + INDICATOR_SIZE, BUFFER_SIZE);
    printf("READ BYTES = %ld\n", read_bytes);
    message.mtext[0] = (char)read_bytes;
    while (read_bytes > 0) {
        msgsnd(msqid, &message, MESSAGE_SIZE, IPC_NOWAIT);

        msgrcv(msqid, &message, MESSAGE_SIZE, 2, 0);
        int bytes_received = (int)message.mtext[0];

        write(fd_out, message.mtext + INDICATOR_SIZE, bytes_received);

        read_bytes = read(fd_in, message.mtext + INDICATOR_SIZE, BUFFER_SIZE);
        message.mtext[0] = (char)read_bytes;
        message.mtype = 1;
    }

    read_bytes = read(fd_in, message.mtext + INDICATOR_SIZE, BUFFER_SIZE);
    message.mtext[0] = (char)read_bytes;
    message.mtype = 1;
    msgsnd(msqid, &message, MESSAGE_SIZE, IPC_NOWAIT);
    
    return 0;
}