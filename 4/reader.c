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

    int pipe_out_fd = atoi(argv[1]);
    char *filename = argv[2];

    printf("Pipe : %d\n", pipe_out_fd);

    int fd = open(filename, O_RDONLY);

    printf("READER is here\n");
    ssize_t read_bytes = read(fd, buffer, BUFFER_SIZE);
    char sent_bytes[INDICATOR_SIZE];
    while (read_bytes > 0) {
        sprintf(sent_bytes, "%ld", read_bytes);
        write(pipe_out_fd, sent_bytes, INDICATOR_SIZE);
        write(pipe_out_fd, buffer, read_bytes);

        read_bytes = read(fd, buffer, BUFFER_SIZE);
    }
    sprintf(sent_bytes, "%ld", read_bytes);
    write(pipe_out_fd, sent_bytes, INDICATOR_SIZE);
    return 0;
}