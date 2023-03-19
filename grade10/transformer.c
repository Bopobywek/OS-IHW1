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
#include <sys/msg.h>


int isVowel(char letter) {
    return letter == 'a' || letter == 'e' || letter == 'i' || letter == 'o' || letter == 'u' || letter == 'y'
        || letter == 'A' || letter == 'E' || letter == 'I' || letter == 'O' || letter == 'U' || letter == 'Y';
}
void replaceLetters(char string[], ssize_t size) {
    for (ssize_t i = 0; i < size; ++i) {
        if (string[i] >= 'a' && !isVowel(string[i])) {
            string[i] = (char) (string[i] - 32);
        }
    }
}

typedef struct {
    long mtype;
    char mtext[MESSAGE_SIZE];
} message_t;

int main(int argc, char *argv[]) {
    int msqid = msgget(MSG_KEY, IPC_CREAT | 0666);

    if (msqid < 0) {
        printf("Can't create message queue\n");
        exit(-1);
    }
    
    message_t received_message;
    msgrcv(msqid, &received_message, MESSAGE_SIZE, 1, 0);
    int bytes_received = received_message.mtext[0];
    printf("RECEIVED BYTES = %d\n", bytes_received);
    while (bytes_received > 0) {
        replaceLetters(received_message.mtext + INDICATOR_SIZE, bytes_received);

        received_message.mtype = 2;
        msgsnd(msqid, &received_message, MESSAGE_SIZE, IPC_NOWAIT);

        msgrcv(msqid, &received_message, MESSAGE_SIZE, 1, 0);
        bytes_received = (int)received_message.mtext[0];
        printf("RECEIVED BYTES = %d\n", bytes_received);
    }
    return 0;
}