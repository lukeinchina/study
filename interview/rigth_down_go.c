/*
 *
 *
 *
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>

int func(int m, int n) {
    if (1 == m || 1 == n) {
        return 1;
    }else {
        return func(m-1,n) + func(m, n-1);
    }
}

int
main(int argc, char *argv[]) {
    int n,m;
    if (argc < 3) {
        printf("usage:%s m n\n", argv[0]);
        exit(1);
    }
    m = atoi(argv[1]);
    n = atoi(argv[2]);
    printf("result is :%d\n", func(m, n));
    return 0;
}
