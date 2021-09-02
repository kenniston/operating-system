#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>

void print_proc(pid_t pid, char* name) { 
    pid_t pgid = getpgid(pid);
    pid_t ppid = getppid();
    pid_t sid = getsid(0);

    printf("Process %s ==> pid: %d, ppid: %d, group: %d, session: %d.\n", name, pid, ppid, pgid, sid);
}

void move_proc(pid_t pid, pid_t ppid, pid_t session) {
    pid_t child_pid = fork();
    if (child_pid == 0) {    
        printf("Moving process...");
        int ret = setpgid(child_pid, ppid);
        if (ret == -1) {
            int err = errno;
            printf("Error moving process: %d - %s.\n", err, strerror(err));
        }
    }
}

int main() {
    print_proc(getpid(), "main");
    getchar();

    pid_t newp;
    pid_t news;
    printf("Type the new parent PID: ");
    scanf("%d", &newp);
    printf("Type the new session PID: ");
    scanf("%d", &news);
    move_proc(getpid(), newp, news);

    print_proc(getpid(), "main");
    getchar();
    
    return EXIT_SUCCESS;
}
