/*
 * =====================================================================================
 *
 *       Filename:  apple.c
 *
 *    Description:  ƽ������N*M�����ӣ�ÿ�������з���һ��������ƻ����
 *                  ������Ͻǵĸ��ӿ�ʼ�� ÿһ��ֻ�������߻��������ߣ�
 *                  ÿ���ߵ�һ�������ϾͰѸ������ƻ���ռ������� ������ȥ����������ռ������ٸ�ƻ����
 *
 *        Version:  1.0
 *        Created:  07/15/2014 03:21:29 PM
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

int d[128][128];

#define COLUMN_SIZE 4


int max_apple(int (*a)[COLUMN_SIZE], int row) {
    int max = 0;
    int i = 0;
    int j = 0;
    /* initialize */
    for (i = 0; i < COLUMN_SIZE; i++) {
        d[0][i] = 0;
        d[i][0] = 0;
    }
    for (i = 0; i < row; i++) {
        for (j = 0; j < COLUMN_SIZE; j++) {
            if (d[i][j+1] < d[i+1][j]) {
                d[i+1][j+1] = a[i][j] + d[i+1][j];
            } else {
                d[i+1][j+1] = a[i][j] + d[i][j+1];
            }
        }
    }
    max = d[i][j];
    return max;
}

int
main(int argc, char *argv[]) {
    int a[4][COLUMN_SIZE] = {{1,3,5,7},
        {2,4,6,8},
        {7,8,9,10},
        {4,4,4,3}
    };
    printf("max:%d\n", max_apple(a, 4));
    return 0;
}
