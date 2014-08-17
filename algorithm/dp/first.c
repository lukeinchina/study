/*
 * =====================================================================================
 *
 *       Filename:  first.c
 *
 *    Description:  共有1，3，5元三种硬币，问凑够11块钱最少需要多少枚硬币
 *
 *        Version:  1.0
 *        Created:  07/13/2014 01:07:50 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  luke (), lukeinchina@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>

int min_coins(int n) {
    int little[6] = {0,1,2,1,2,1};
    if (n <= 5)  {
        return little[n];
    }
    if (n < 0 ) { 
        return -1;
    }

    int min = 0;
    int *d = (int *)malloc((n+1) * sizeof(int));
    int i = 0;
    for(i = 0; i <= 5; i++) {
        d[i] = little[i];
    }
    for (i = 6; i <= n; i++) {
        d[i] = d[i-1] + 1;
        if (d[i] > d[i-3] + 1) {
            d[i] = d[i-3] + 1;
        }
        if (d[i] > d[i-5] + 1) {
            d[i] = d[i-5] + 1;
        }
    }
    min = d[n];
    free(d);
    return min;
}

int
main(int argc, char *argv[]) {
    int min = min_coins(3); 
    printf("f(3) = %d", min);
    if ( 1 != min) {
        printf(":error\n");
    } else {
        printf(":ok\n");
    }

    min = min_coins(30);
    printf("f(30) = %d", min);
    if ( 6 != min) {
        printf(":error\n");
    } else {
        printf(":ok\n");
    }

    min = min_coins(103);
    printf("f(103) = %d", min);
    if ( 21 != min) {
        printf(":error\n");
    } else {
        printf(":ok\n");
    }

    return 0;
}
