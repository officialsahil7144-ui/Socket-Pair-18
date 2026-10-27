#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/wait.h> 
#include <unistd.h> 
int main(void) { 
    int sv[2]; 
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv) == -1) { perror("socketpair"); return 1; } 
    pid_t pid = fork(); 
    if (pid < 0) { perror("fork"); return 1; } 
    if (pid == 0) { 
        close(sv[0]); 
        char buf[100]; 
        ssize_t n = read(sv[1], buf, sizeof buf - 1); 
        if (n < 0) { perror("read"); _exit(1); } 
        buf[n] = '\\0'; 
        dprintf(STDOUT_FILENO, "Child received: %s\\n", buf); 
        close(sv[1]); 
        _exit(0); 
    } 
    close(sv[1]); 
    const char *msg = "IPC socketpair message"; 
    if (write(sv[0], msg, strlen(msg)) < 0) perror("write"); 
    close(sv[0]); 
    waitpid(pid, NULL, 0); 
    return 0; 
} 
