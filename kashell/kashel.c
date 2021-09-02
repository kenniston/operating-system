#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <ctype.h>

#define MAX_CMD_SIZE 4096

typedef struct task {
    struct task *next;
    char cmd[MAX_CMD_SIZE];
} task_t;


void str_trim(char * s) {
    char *start = s;
    int length = strlen(start);

    while (isspace(start[length - 1])) {
        start[--length] = 0;
    }
    while (*start && isspace(*start)) {
        ++start, --length;
    }
    memmove(s, start, length + 1);
}

task_t* create_pipeline(char *str) {
    task_t *head = NULL;
    task_t *cur = NULL;
    char *cmd;
    char *cmdstr = malloc(strlen(str) + 1);

    strcpy(cmdstr, str);
    while ((cmd = strsep(&cmdstr, "|"))) {
        task_t *t = malloc(sizeof(task_t));
        if (t != NULL) {
            head = head ?: t;
            if (cur != NULL) {
                cur->next = t;
            }
            str_trim(cmd);
            strcpy(t->cmd, cmd);
            cur = t;
        } else {
            //TODO: Error creating command. Stop pipeline.
        }
    }
    return head;
}

void print_pipeline(task_t *tasks) {
    task_t *tmp;
    task_t *cur = tasks;

    printf("Pipeline:\n");
    while (cur) {
        tmp = cur->next;
        printf("\t=> %s\n", cur->cmd);
        cur = tmp;
    }
}

void free_pipeline(task_t *tasks) {
    task_t *tmp;
    task_t *cur = tasks;
    while (cur) {
        tmp = cur->next;
        free(cur);
        cur = tmp;
    }
}

void read_cmd_loop() {
    do {
        printf("cmd> ");

        char *cmd = 0;
        size_t cmdlen = 0;
        getline(&cmd, &cmdlen, stdin);

        if (strcasecmp(cmd, "quit") == 0) {
            break;
        }

        task_t *tasks = create_pipeline(cmd);
        print_pipeline(tasks);

        free_pipeline(tasks);
        free(cmd);
    } while (true);
}

void print_welcome() {
    printf("Welcome to the ka-shell.\n");
}

int main() {
    print_welcome();
    read_cmd_loop();

	return 0;
}
