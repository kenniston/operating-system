#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

#define MEMORY_SIZE 100

int mem[MEMORY_SIZE] = {-1};
int acc = -1;
int ip = 0;
int stack_size;
bool running = true;
int code_size;

void halt() {
    running = false;
}

void op_read(int position) {
    scanf("%d", &mem[position]);
}

void op_write(int position) {
    printf("%d\n", mem[position]);
}

void op_load(int position) {
    acc = mem[position];
}

void op_store(int position) {
    mem[position] = acc;
}

void op_add(int position) {
    acc += mem[position];
}

void op_sub(int position) {
    acc = mem[position] - acc;
}

void op_div(int position) {
    if (acc == 0) {
        printf("error: div by zero.\n");
        halt();
    }
    acc = mem[position] / acc;
}

void op_mul(int position) {
    acc *= mem[position]; 
}

void op_branch(int position) {
    ip = position;
}

void op_branch_neg(int position) {
    if (acc < 0) {
        ip = position;
    }
}

void op_branch_zero(int position) {
    if (acc == 0) {
        ip = position;
    }
}

void load_file(char *file) {
    FILE *f;
    f = fopen(file, "r");
    if (f == NULL) {
        printf("Invalid program file!\n");
        exit(EXIT_FAILURE);
    }

    ssize_t read;
    size_t len;
    char *line;
    while ((read = getline(&line, &len, f)) != -1) {
        int command = atoi(line);
        if (command <= -9999 || ip == MEMORY_SIZE) {
            break;
        }
        mem[ip++] = command;
    }
    fclose(f);
    free(line);
}

void bootloader(char *file) {
    if (file != NULL) {
        printf("Load from file: %s\n", file);
        load_file(file);
    } else {
        int command;
        do {
             scanf("%d", &command);
             if (command <= -9999 || ip == MEMORY_SIZE) {
                break;
             }
    
             mem[ip++] = command;
        } while (true);
   }
    stack_size = MEMORY_SIZE - ip;
    code_size = ip;
    ip = 0;
     
#ifdef DEBUG
    printf("\n\n");
    printf("Program Loaded!\n");
    printf("Instruction Count: %d\n", code_size);
    printf("Stack Size: %d\n", stack_size);
    printf("Total Memory: %d\n\n", MEMORY_SIZE);
#endif
}

void dump() {
    printf("\n");

    char vacc[5] = "----";
    if (acc > -1) {
        sprintf(vacc, "%04d", acc);
    }
    printf("acc: %s\n", vacc);

    char vip[5] = "----";
    if (ip > -1) {
        sprintf(vip, "%04d", ip);
    }
    printf("ip : %s\n", vip);
    printf("\n");

    for (int i = 0; i < MEMORY_SIZE; i++) {
        char vl[5] = "----";
        if (mem[i] > -1) {
            sprintf(vl, "%04d", mem[i]);
        }
        printf("%02d: %s   %s", i, vl, (i+1) % 10 == 0? "\n": "");
    }
    printf("\n\n");
}

void run() {
    while (running) {
        int command = mem[ip++];

        int op = command / 100;
        int vl = command % 100;

        switch (op) {
            case 10: op_read(vl); break;
            case 11: op_write(vl); break;
            case 20: op_load(vl); break;
            case 21: op_store(vl); break;
            case 30: op_add(vl); break;
            case 31: op_sub(vl); break;
            case 32: op_div(vl); break;
            case 33: op_mul(vl); break;
            case 40: op_branch(vl); break;
            case 41: op_branch_neg(vl); break;
            case 42: op_branch_zero(vl); break;
            case 43: halt(); break;
        }

        if (command == code_size -1) {
            halt();
        }
    }

}

int main(int argc, char *args[]) {
    char *file = argc > 1 ? args[1] : NULL;

    memset(mem, -1, MEMORY_SIZE * sizeof(int));

    bootloader(file);
    run();
    dump();

    return EXIT_SUCCESS;
}
