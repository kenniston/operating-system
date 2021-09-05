#include <stdio.h>
#include <stdlib.h>

int main() {
    char str[100];
    while (scanf("%s", str) == 1) {
        printf("[child] read '%s' from stdin\n", str);
    }

    return EXIT_SUCCESS;
}
