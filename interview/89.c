#include <stdio.h>
#include <stdlib.h>

int* grayCode(int n, int* returnSize) {
    int i = 0, total = 1 << n;
    int *grays;
    grays = (int *)malloc(sizeof(i) * total);
    for (i = 0; i < total; i++) {
        grays[i] = i ^ (i >> 1);
    }
    *returnSize = total;
    return grays;
}

int
main(int argc, char *argv[]) {
    int i = 0;
    printf("\n");
    return 0;
}
