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


char buffer[BUFFER_SIZE];
char sent_bytes[INDICATOR_SIZE];

void continueIfFileCouldBeOpened(char *filename, int status) {
    if (status < 0) {
        printf("Can't open file %s\n", filename);
        exit(-1);
    }
}

int getAvailableBytes(int pipe_in_fd) {
    read(pipe_in_fd, buffer, INDICATOR_SIZE);
    return atoi(buffer);
}

int main(int argc, char *argv[]) {
    mknod(IT_PIPE, S_IFIFO | 0666, 0);
    mknod(TI_PIPE, S_IFIFO | 0666, 0);

    int pipe_out_fd = open(IT_PIPE, O_WRONLY);
    int pipe_in_fd = open(TI_PIPE, O_RDONLY);

    if (pipe_in_fd < 0 || pipe_out_fd < 0) {
        perror("Can't open pipes in io process\n");
        exit(-1);
    }

    printf("IN_FD: %d, OUT_FD: %d\n", pipe_in_fd, pipe_out_fd);

    char *filename_in = argv[1];
    char *filename_out = argv[2];

    int fd_in = open(filename_in, O_RDONLY);
    continueIfFileCouldBeOpened(filename_in, fd_in);

    int fd_out = open(filename_out, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    continueIfFileCouldBeOpened(filename_out, fd_out);

    ssize_t read_bytes = read(fd_in, buffer, BUFFER_SIZE);
    while (read_bytes > 0) {
        printf("READ_BYTES = %ld\n", read_bytes);
        sprintf(sent_bytes, "%ld", read_bytes);
        write(pipe_out_fd, sent_bytes, INDICATOR_SIZE);
        write(pipe_out_fd, buffer, read_bytes);

        int bytes_received = getAvailableBytes(pipe_in_fd);
        read_bytes = read(pipe_in_fd, buffer, bytes_received);
        write(fd_out, buffer, read_bytes);

        read_bytes = read(fd_in, buffer, BUFFER_SIZE);
    }

    sprintf(sent_bytes, "%ld", read_bytes);
    write(pipe_out_fd, sent_bytes, INDICATOR_SIZE);
    
    return 0;
}