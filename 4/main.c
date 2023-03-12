#include <stdio.h>

#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>

int createReader(char *pipe_out, char *filename) {
    pid_t chpid = fork();
    if (chpid == 0) {
        (void) execlp("./reader", "./reader", pipe_out, filename, 0);
        exit(0);
    }

    return chpid;
}

int createTransformer(char *pipe_in, char *pipe_out) {
    pid_t chpid = fork();
    if (chpid == 0) {
        (void) execlp("./transformer", "./transformer", pipe_in, pipe_out, 0);
        exit(0);
    }

    return chpid;
}

int createWriter(char *pipe_in, char *filename) {
    pid_t chpid = fork();
    if (chpid == 0) {
        (void) execlp("./writer", "./writer", pipe_in, filename, 0);
        exit(0);
    }

    return chpid;
}

int main(int argc, char *argv[]) {
    char reader_out[128];
    char transformer_in[128];
    char transformer_out[128];
    char writer_in[128];

    int fd1[2];
    int fd2[2];
    pipe(fd1);
    pipe(fd2);

    sprintf(reader_out, "%d", fd1[1]);
    sprintf(transformer_in, "%d", fd1[0]);
    sprintf(transformer_out, "%d", fd2[1]);
    sprintf(writer_in, "%d", fd2[0]);

    pid_t reader_pid = createReader(reader_out, argv[1]);
    pid_t transformer_pid = createTransformer(transformer_in, transformer_out);
    pid_t writer_pid = createWriter(writer_in, argv[2]);

    int status = 0;
    waitpid(reader_pid, &status, 0);
    waitpid(transformer_pid, &status, 0);
    waitpid(writer_pid, &status, 0);

    close(fd1[0]);
    close(fd1[1]);
    close(fd2[0]);
    close(fd2[1]);
    return 0;
}

int isVowel(char letter) {
    return letter == 'a' || letter == 'e' || letter == 'i' || letter == 'o' || letter == 'u'
        || letter == 'A' || letter == 'E' || letter == 'I' || letter == 'O' || letter == 'U';
}

void replaceLetters(char string[]) {
    for (int i = 0; string[i] != '\0'; ++i) {
        if (string[i] >= 'a' && !isVowel(string[i])) {
            string[i] = (char) (string[i] - 32);
        }
    }
}