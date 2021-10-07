#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main () {
    char str[101];
    char *command;
    char *param;

    int count = 0;

    while (count++ < 10) {
        printf("Digite uma linha comando: ");
        fgets(str, 100, stdin);

        // Cria uma copia da linha de comando. A funçao strsep altera a string de entrada.
        char *paramstr = calloc(1, strlen(str) + 1);
        strcpy(paramstr, str);

        // Quebra a linha por pipe
        int cmd_count = 1;
        while ((command = strsep(&paramstr, "|"))) {
            printf("\tComando %d:\n", cmd_count++);
            // Quebra o comando por espaço
            while ((param = strsep(&command, " "))) {
                printf("\t\t%s\n", param);
            }
        }
    }

    return(0);
}