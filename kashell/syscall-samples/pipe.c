#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main() {
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("[parent] Error creating pipe");
        exit(EXIT_FAILURE);
    }

    pid_t child_pid = fork();
    if (child_pid == 0) { // child process 
        close(pipefd[1]);

        if (dup2(pipefd[0], STDIN_FILENO) == -1) {
            perror("[parent] Error setting stdin for child process");
        }

        printf("[parent] Replacing pipe image to pipeout on child process...\n");
        char *argv[] = {"", NULL};
        int ret = execvp("./pipeout", argv);
        if (ret != 0) {
    		perror("[parent] Error running 'pipeout'");
    		return 1;
    	}
        printf("Never executed!");
    } else { // parent process
        close(pipefd[0]);
        write(pipefd[1], "first ", 6);
        write(pipefd[1], "second", 6);
        close(pipefd[1]);

        int wstatus;
        if (wait(&wstatus) == -1) {
            perror("[parent] Error waiting child process");
            exit(EXIT_FAILURE);
        }

        if (WIFEXITED(wstatus)) {
            printf("[parent] exited, status=%d\n", WEXITSTATUS(wstatus));
        } else if (WIFSIGNALED(wstatus)) {
            printf("[parent] killed by signal %d\n", WTERMSIG(wstatus));
        } else if (WIFSTOPPED(wstatus)) {
            printf("[parent] stopped by signal %d\n", WSTOPSIG(wstatus));
        } else if (WIFCONTINUED(wstatus)) {
            printf("[parent] continued\n");
        }
    }

    return EXIT_SUCCESS;
}