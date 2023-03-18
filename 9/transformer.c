#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "constants.h"
#include <semaphore.h>
#include <sys/wait.h>

int isVowel(char letter) {
    return letter == 'a' || letter == 'e' || letter == 'i' || letter == 'o' || letter == 'u'
        || letter == 'A' || letter == 'E' || letter == 'I' || letter == 'O' || letter == 'U';
}

void replaceLetters(char string[], ssize_t size) {
    for (ssize_t i = 0; i < size; ++i) {
        if (string[i] >= 'a' && !isVowel(string[i])) {
            string[i] = (char) (string[i] - 32);
        }
    }
}

char buffer[BUFFER_SIZE];
char sent_bytes[INDICATOR_SIZE];

int getAvailableBytes(int pipe_in_fd) {
    read(pipe_in_fd, buffer, INDICATOR_SIZE);
    return atoi(buffer);
}


int main(int argc, char *argv[]) {
    mknod(IT_PIPE, S_IFIFO | 0666, 0);
    mknod(TI_PIPE, S_IFIFO | 0666, 0);

    int pipe_in_fd = open(IT_PIPE, O_RDONLY);
    int pipe_out_fd = open(TI_PIPE, O_WRONLY);

    printf("IN_FD: %d, OUT_FD: %d\n", pipe_in_fd, pipe_out_fd);

    int bytes_received = getAvailableBytes(pipe_in_fd);
    printf("RECEIVED BYTES = %d\n", bytes_received);
    ssize_t read_bytes = read(pipe_in_fd, buffer, bytes_received);
    while (read_bytes > 0) {
        replaceLetters(buffer, read_bytes);

        sprintf(sent_bytes, "%ld", read_bytes);
        write(pipe_out_fd, sent_bytes, INDICATOR_SIZE);

        write(pipe_out_fd, buffer, read_bytes);
        bytes_received = getAvailableBytes(pipe_in_fd);
        read_bytes = read(pipe_in_fd, buffer, bytes_received);
    }

    close(pipe_in_fd);
    close(pipe_out_fd);
    return 0;
}