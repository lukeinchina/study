/*
 * =====================================================================================
 *
 *       Filename:  second.c
 *
 *    Description:  longest increasing subsequence. Or
 *                  longest non-descreasing subsequence.
 *
 *        Version:  1.0
 *        Created:  07/15/2014 11:56:17 AM
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
#include <string.h>

int LIS(const int *seq, const int length) {
    if (NULL == seq || length < 1) {
        return -1;
    }
    int max = 0;
    int *d = (int *)malloc(length * sizeof(int));
    d[0] = 1;
    /* d(i)±íÊ¾µ½ÒÔseq[i]Îª½áÊøµÄ×î³¤µÝÔö×ÓÐòÁÐÊµÄ³¤¶È */
    int i = 0;
    int j = 0;
    for (i = 1; i < length; i++) {
        for (j = i; j >= 0; j--) {
            if (seq[i] > seq[j]) {
                d[i] = d[j] + 1;
                break;
            }
        }
        if (j < 0) {
            d[i] = 1;
        }
    }
    for (i = 0; i < length; i++) {
        printf("%d\t", seq[i]);
    }
    printf("\n");
    for (i = 0; i < length; i++) {
        if (max < d[i]) {
            max = d[i];
        }
        printf("%d\t", d[i]);
    }
    printf("\n");
    free(d);
    return max;
}

int
main(int argc, char *argv[]) {
    int a[6] = {5, 3, 4, 8, 6, 7};
    printf("max:%d\n", LIS(a, 6));
    return 0;
}
