#include <stdio.h>
#include <unistd.h>

int main() {
	printf("before execvp: pid(%d)\n", getpid());

	char *argv[] = {"ls", "-la", NULL};

    int ret = execvp("ls", argv);
    if (ret != 0) {
    	perror("Error running 'ls': ");
    	return 1;
    }

    printf("Never executed!\n");

	return 0;
}	
