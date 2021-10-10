#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void *runner(void *param);

int main(int argc, char *argv[]) {
    int *result;

    pthread_t tid;
    pthread_attr_t attr;

    printf("Main thread starting...\n");

    pthread_attr_init(&attr);
    pthread_create(&tid, &attr, runner, argv[1]);
    pthread_join(tid, (void **) &result);

    printf("Second thread result: %d.\n", *result);
    printf("Main thread finished.\n");

    return EXIT_SUCCESS;
}

void *runner(void *param) {
    printf("Starting second thread...\n");
    int upper = atoi(param);
    int *sum = malloc(sizeof(int));

    for (int i = 0; i < upper; i++) {
        *sum += i;
    }

    printf("Second thread finished.\n");
    pthread_exit((void *) sum);
}