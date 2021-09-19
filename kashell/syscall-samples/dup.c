#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char **argv) {
    int in, out;
    char *grep_args[] = {"grep", "teste", NULL};

    in = open("in.txt", O_RDONLY);
    out = open("out.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);

    int r1 = dup2(in, STDIN_FILENO);
    if (r1 == -1) {
        printf("ERROR INPUT");
    }

    int r2 = dup2(out, STDOUT_FILENO);
    if (r2 == -1) {
        perror("ERROR OUTPUT");
    }
    close(in);
    close(out);

    execvp("grep", grep_args);
}
