/**0-1 knapsack d(i, j)表示前i个物品装到剩余容量为j的背包中的最大重量**/
#include <stdio.h>


#define MAXN 128
#define MAXV 1024
#define max(a,b) (a) > (b) ? (a) : (b)

/*int V[MAXN], W[MAXN];
int d[MAXN][MAXC];
*/
int d[MAXN][MAXV];

int knapsack(int V[], int W[], int count, int C) {
    int i = 0;
    int j = 0;
    for (i = 0; i < MAXV; i++) {
        d[0][i] = 0;
    }
    for (i = 0; i < MAXN; i++) {
        d[i][0]= 0;
    }
    for (i = 1; i <= count; i++) {
        for (j = 1; j <= C; j++) {
            if (j - V[i-1] >= 0) {
                d[i][j] = max(d[i-1][j], d[i-1][j-V[i-1]] + W[i-1]);
            }else{
                d[i][j] = d[i-1][j];
            }
        }
    }
    for (i = 0; i <= count; i++) {
        for (j = 0; j <= C; j++) {
            printf("%d\t", d[i][j]);
        }
        printf("\n");
    }
    return d[count][C];
}

int
main(void) {
    int V[MAXN] = {5, 4, 3};
    int W[MAXN] = {20, 10, 12};
    printf("result:%d\n", knapsack(V, W, 3, 10));
    return 0;
}
