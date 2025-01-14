#include <unistd.h>
#include <stdio.h>

int main() {
    int pid, status;
    pid = fork();
    if (pid == 0) {
        // Child process
        printf("Child process\n");
        char *argv[] = {"echo", "THIS", "IS", "CHILD", NULL};
    }
}