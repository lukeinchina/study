#include <stdio.h>
#include <stdlib.h>

int 
right_down_go(int n, int m) {
    if (1 == n || 1 == m) {
        return 1;
    } else {
        return right_down_go(n-1, m) + right_down_go(n, m-1);
    }
}

int
main(int argc, char *argv[]) {
    int n,m;
    if (argc != 3) {
        printf("usage:%s m n\n", argv[0]);
        return -1;
    }
    n = atoi(argv[1]);
    m = atoi(argv[2]);
    printf("%d\n", right_down_go(n, n));
    return 0;
}
