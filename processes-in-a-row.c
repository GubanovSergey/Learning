#include <stdio.h>
#include <stdlib.h>

/*
 * Ок, хорошо. Ровно такое решение и предполагалось.
 */

int main() {
    pid_t pid = 0;
    int i = 0, N = 3;
    int status = -1;
    for (i = 0; i < N + 1; i++) {
        if (pid > 0) {
            wait(&status);
        }
        if (pid == 0) {
            printf("Proccess with ID = %d is in run\nParents ID is %d\n\n", getpid(), getppid());
            if (i < N)
                pid = fork();
        }
    }
    printf("Proccess with ID = %d is finishing\nParents ID is %d\nChild returned %d\n\n", getpid(), getppid(), status >> 8);
    return 0;
}

