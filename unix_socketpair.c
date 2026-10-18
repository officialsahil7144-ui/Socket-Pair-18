#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <string.h>

int main(void) {
    int sv[2];
    pid_t pid;
    char message[] = "Hello through socketpair";
    char buffer[100] = {0};

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv) == -1) {
        perror("socketpair");
        return 1;
    }

    pid = fork();

    if (pid == -1) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        close(sv[0]);

        ssize_t bytes = read(sv[1], buffer, sizeof(buffer) - 1);
        if (bytes == -1) {
            perror("read");
            close(sv[1]);
            _exit(1);
        }

        buffer[bytes] = '\0';
        printf("Child received: %s\n", buffer);
        close(sv[1]);
        _exit(0);
    } else {
        close(sv[1]);

        if (write(sv[0], message, strlen(message)) == -1) {
            perror("write");
            close(sv[0]);
            wait(NULL);
            return 1;
        }

        close(sv[0]);
        wait(NULL);
        printf("Parent sent the message successfully.\n");
    }

    return 0;
}
