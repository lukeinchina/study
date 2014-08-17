/*
 * =====================================================================================
 *
 *       Filename:  binary_heap_test.c
 *
 *    Description:  G
 *
 *        Version:  1.0
 *        Created:  08/13/2014 11:20:15 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  luke (), lukeinchina@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "binary_heap.h"
#define QueueSize 10

int
main(void) {
    int i = 0;
    int value = 0;
    PriorityQueue queue = Initialize(QueueSize);
    srand(time(NULL));
    for (i = 0; i < QueueSize; i++) {
        value = rand() % 100;
        Insert(queue, value);
        printf("%d\t", value);
    }
    printf("\n");
    /*
    for (i = 0; i < QueueSize; i++) {
        printf("%d\t", queue->elements[i]);
    }  */
    printf("\n");
    for (i = 0; i < QueueSize; i++) {
        value = DeleteMin(queue);
        printf("%d\t", value);
    }
    printf("\n");
    Destory(queue);
    return 0;
}
