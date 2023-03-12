#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "constants.h"

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

int main(int argc, char *argv[]) {

    int pipe_in_fd = atoi(argv[1]);
    int pipe_out_fd = atoi(argv[2]);

    printf("Transformer is here\n");
    read(pipe_in_fd, buffer, INDICATOR_SIZE);
    int bytes_received = atoi(buffer);
    ssize_t read_bytes = read(pipe_in_fd, buffer, bytes_received);
    char sent_bytes[10];
    while (read_bytes > 0) {
        printf("Transformer is here2, bytes %ld\n", read_bytes);
        replaceLetters(buffer, read_bytes);

        sprintf(sent_bytes, "%ld", read_bytes);
        write(pipe_out_fd, sent_bytes, 10);
        write(pipe_out_fd, buffer, read_bytes); // \0 ???

        read(pipe_in_fd, buffer, INDICATOR_SIZE);
        bytes_received = atoi(buffer);
        read_bytes = read(pipe_in_fd, buffer, bytes_received);
    }

    printf("TRANSFORMER EXIT\n");
    return 0;
}