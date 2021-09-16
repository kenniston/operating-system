#include <stdio.h>
#include <stdlib.h>

int sum(int a, int b) {
    return a + b;
}

int mul(int a, int b) {
    return a * b;
}

int sub(int a, int b) {
    return a - b;
}

int divide(int a) {
    return a;
}

int (*fp)(int);

int main() {
    fp = sum;
    int r = fp(2);
    printf("result: %d \n", r);

    return EXIT_SUCCESS;
}
