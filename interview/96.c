#include <stdio.h>
#include <stdlib.h>

int numTrees(int n) {
    int a[128] = {1, 1, 2, 5};
    int i,j;
    for (i = 3; i <= n; i++) {
        a[i] = 0;
        for (j = 0; j < i; j++) {
            a[i] += a[j] * a[i-1-j];
        }
    }
    return a[n];
}

int
main(int argc, char *argv[]) {
    int n = 3;
    if (argc < 2) {
        printf("usage:%s n\n", argv[0]);
        return 1;
    }
    n = atoi(argv[1]);
    printf("%d node's trees:%d\n", n, numTrees(n));
    return 0;
}
