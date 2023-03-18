#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "constants.h"


#define IT_PIPE "io-to-transformer.pipe"
#define TI_PIPE "transformer-to-io.pipe"

int ioManipulator(char *pipe_in, char *pipe_out, char *filename_in, char *filename_out) {
    pid_t chpid = fork();
    if (chpid == 0) {
        (void) execlp("./io_manipulator", "./io_manipulator", pipe_in, pipe_out, filename_in, filename_out, NULL);
        exit(0);
    }

    return chpid;
}

int createTransformer(char *pipe_in, char *pipe_out) {
    pid_t chpid = fork();
    if (chpid == 0) {
        (void) execlp("./transformer", "./transformer", pipe_in, pipe_out, NULL);
        exit(0);
    }

    return chpid;
}

void continueIfProcessStart(pid_t pid, char *process_name) {
    if (pid >= 0) {
        return;
    }

    printf("Can't run %s process\n", process_name);
    exit(-1);
}

void continueIfPipeIsOpen(int status) {
    if (status < 0) {
        printf("Can't open pipe\n");
        exit(-1);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Only 2 arguments expected: <filename_in> <filename_out>\n");
        exit(-1);
    }

    mknod(IT_PIPE, S_IFIFO | 0666, 0);
    mknod(TI_PIPE, S_IFIFO | 0666, 0);

    pid_t manipulator_pid = ioManipulator(TI_PIPE, IT_PIPE, argv[1], argv[2]);
    continueIfProcessStart(manipulator_pid, "manipulator");

    pid_t transformer_pid = createTransformer(IT_PIPE, TI_PIPE);
    continueIfProcessStart(transformer_pid, "transformer");

    int status = 0;
    waitpid(manipulator_pid, &status, 0);
    waitpid(transformer_pid, &status, 0);

    return 0;
}