#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

// Input and Ouput Operations
#define READ 10
#define WRITE 11

// Load and Store operations
#define LOAD 20
#define STORE 21

// Arithmetic operations
#define ADD 30
#define SUBTRACT 31
#define DIVIDE 32
#define MULTIPLY 33

// Transfer of Control operations
#define BRANCH 40
#define BRANCHNEG 41
#define BRANCHZERO 42
#define HALT 43

// End of Program instruction
#define END_PROGRAM -9999

// Simpletron Memory Size
#define MEMORY_SIZE 100

int mem[MEMORY_SIZE] = {-1};    // Simpletron memory
int acc = -1;                   // accumulator register
int ip = 0;                     // instruction pointer register
int stack_size;                 // Simpletron stack size
int code_size;                  // program size in memory
bool running = true;            

void halt() {
    running = false;
}

void op_read(int position) {
    printf("[%02d] ? Data = ", position);
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
    if (mem[position] == 0) {
        fprintf(stderr, "error: div by zero.\n");
        halt();
    }
    acc /= mem[position];
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

void invalid_instruction(int position, int instruction) {
    printf("*** Invalid instruction (%04d) at address [%02d]. ***\n*** Simpletron execution terminated!            ***\n\n", instruction, position);
    halt();
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
        if (command <= END_PROGRAM || ip == MEMORY_SIZE) {
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
            printf("[%02d] ? Command = ", ip);
            scanf("%d", &command);
            if (command <= END_PROGRAM || ip == MEMORY_SIZE) {
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
            case READ: op_read(vl); break;
            case WRITE: op_write(vl); break;
            case LOAD: op_load(vl); break;
            case STORE: op_store(vl); break;
            case ADD: op_add(vl); break;
            case SUBTRACT: op_sub(vl); break;
            case DIVIDE: op_div(vl); break;
            case MULTIPLY: op_mul(vl); break;
            case BRANCH: op_branch(vl); break;
            case BRANCHNEG: op_branch_neg(vl); break;
            case BRANCHZERO: op_branch_zero(vl); break;
            case HALT: halt(); break;
            default: invalid_instruction(ip-1, command);
        }

        if (command == code_size -1) {
            halt();
        }
    }
}

void start() {
    printf("*** Welcome to Simpletron!                                ***\n");
    printf("*** Please insert one instruction (or data word)          ***\n");
    printf("*** at a time into your program. I'll type the location   ***\n");
    printf("*** number and a question mark (?) and then you type    ***\n");
    printf("*** the instruction for the allocation.                   ***\n");
    printf("*** Type the -9999 number to indicate the end of your     ***\n");
    printf("*** program.                                              ***\n\n");
}

int main(int argc, char *args[]) {
    char *file = argc > 1 ? args[1] : NULL;

    memset(mem, -1, MEMORY_SIZE * sizeof(int));
    
    if (!file) {
        start();
    }

    bootloader(file);
    run();
    dump();

    return EXIT_SUCCESS;
}
