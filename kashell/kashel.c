#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <ctype.h>

#define MAX_CMD_SIZE 512
#define MAX_PARAMS 255
#define MAX_OUT_FILE_PATH 255
#define MAX_IN_FILE_PATH 255
#define OUT_SUBCMD ">"
#define IN_SUBCMD "<"

typedef struct task {
    struct task *next;
    char cmd[MAX_CMD_SIZE];
    char *params[MAX_PARAMS];
    char outfile[MAX_OUT_FILE_PATH];
    char infile[MAX_IN_FILE_PATH];
} task_t;


void str_trim(char * s) {
    char *start = s;
    ulong length = strlen(start);

    while (isspace(start[length - 1])) {
        start[--length] = 0;
    }
    while (*start && isspace(*start)) {
        ++start, --length;
    }
    memmove(s, start, length + 1);
}

void parse_cmd_params(task_t *task, char *params) {
    char *paramstr = calloc(1, strlen(params) + 1);
    strcpy(paramstr, params);

    char *out = strstr(paramstr, OUT_SUBCMD);
    if (out != NULL) {
        strncpy(task->outfile, out + sizeof(OUT_SUBCMD), sizeof(task->outfile));
        str_trim(task->outfile);
    }

    ulong length = out ? strlen(out) : 0;
    char *cmd = calloc(1, strlen(paramstr) - length + 1);
    strncpy(cmd, paramstr, strlen(paramstr) - length);
    str_trim(cmd);

    char *param;
    int count = 0;
    while ((param = strsep(&cmd, " "))) {
        if (strcmp(task->cmd, "") == 0) {
            strcpy(task->cmd, param);
            continue;
        }
        task->params[count++] = param;
    }
}

task_t* create_pipeline(char *str) {
    task_t *head = NULL;
    task_t *cur = NULL;
    char *cmd = NULL;
    char *cmdstr = calloc(1, strlen(str) + 1);

    strcpy(cmdstr, str);
    while ((cmd = strsep(&cmdstr, "|"))) {
        task_t *t = calloc(1, sizeof(task_t));
        if (t != NULL) {
            head = head ?: t;
            if (cur != NULL) {
                cur->next = t;
            }
            str_trim(cmd);
            parse_cmd_params(t, cmd);
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
        printf("\t=> %s", cur->cmd);
        size_t n = sizeof cur->params / sizeof *cur->params;

        char *p = NULL;
        int count = 0;
        while ((p = cur->params[count++]) != NULL) {
            printf(" %s", p);
        }

        if (strcmp(cur->outfile, "") != 0) {
            printf(" > %s", cur->outfile);
        }
        printf("\n");
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
        str_trim(cmd);

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
