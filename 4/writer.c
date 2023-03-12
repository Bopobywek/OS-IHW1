#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "constants.h"

char buffer[BUFFER_SIZE];

int main(int argc, char *argv[]) {

    int pipe_in_fd = atoi(argv[1]);
    char *filename = argv[2];

    int fd_output = open(filename, O_WRONLY | O_CREAT | O_TRUNC);
    printf("WRITER is here\n");

    read(pipe_in_fd, buffer, INDICATOR_SIZE);
    int bytes_received = atoi(buffer);
    ssize_t read_bytes = read(pipe_in_fd, buffer, bytes_received);
    while (read_bytes > 0) {
        write(fd_output, buffer, read_bytes); // \0 ???

        read(pipe_in_fd, buffer, INDICATOR_SIZE);
        bytes_received = atoi(buffer);
        read_bytes = read(pipe_in_fd, buffer, bytes_received);
    }

    close(fd_output);
    printf("WRITER EXIT\n");

    return 0;
}