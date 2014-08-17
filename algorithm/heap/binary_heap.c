/*
 * =====================================================================================
 *
 *       Filename:  binary_heap.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/21/2014 09:58:42 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  luke (), lukeinchina@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include "binary_heap.h"

const static int MinPriorityQueueSize = 3;
const static int MinData = (1<<31);

struct HeapStruct
{
    int capacity;
    int size;
    ElementType *elements;
};

PriorityQueue Initialize(int MaxElements)
{
    PriorityQueue heap;
    if (MaxElements < MinPriorityQueueSize) 
    {
        return NULL;
    }
    heap = (PriorityQueue)malloc(sizeof(struct HeapStruct));
    if (NULL == heap)
    {
        return NULL;
    }
    heap->elements = (ElementType *)malloc((MaxElements+1) * sizeof(ElementType));
    if (NULL == heap->elements)
    {
        free(heap);
        return NULL;
    }
    heap->capacity    = MaxElements;
    heap->size        = 0;
    heap->elements[0] = MinData;

    return heap;
};

int IsFull(const PriorityQueue heap) 
{
    return heap->size >= heap->capacity;
}

int IsEmpty(const PriorityQueue heap) 
{
    return heap->size <= 0;
}

void MakeEmpty(PriorityQueue heap)
{
    heap->size = 0;
}

void Destory(PriorityQueue heap)
{
    heap->capacity    = 0;
    heap->size        = 0;
    free(heap->elements);
    heap->elements = NULL;
    return;
}

/*
 * @brief: 小顶堆
 *
 */
int
Insert(PriorityQueue heap, ElementType x)
{
    int i = 0;
    int parent = 0;
    if (IsFull(heap))
    {
        return -1;
    }
    i = heap->size;
    parent = (i - 1) / 2;

    while (i > 0 && heap->elements[parent] > x)
    {
        heap->elements[i] = heap->elements[parent];

        i = parent;
        parent = (i - 1) / 2;
    }
    heap->elements[i] = x;

    heap->size += 1;
    return 0;

    /* 第0个元素是哑元素，减少后面代码的比较次数的写法
    for (i = ++heap->size; heap->elements[i / 2] > x; i /= 2)
    {
        heap->elements[i] = heap->elements[i / 2];
    }
    heap->elements[i] = x;
    */
}

ElementType DeleteMin(PriorityQueue heap)
{
    int i     = 0;
    int child = 0;
    ElementType ret = MinData;
    ElementType last = heap->elements[heap->size-1]; // last

    if (heap->size <= 0 || NULL == heap->elements) {
        return MinData;
    } else {
        ret = heap->elements[0];
    }
    heap->size -= 1;

    heap->elements[0] = last;
    while (i < heap->size) {
        child = i * 2 + 1;
        if (child >= heap->size) {
            break;
        }
        if (child + 1 < heap->size && heap->elements[child] > heap->elements[child+1]) {
            child += 1;
        }
        if (heap->elements[child] < last) {
            heap->elements[i] = heap->elements[child];
        }else {
            break;
        }
        i = child;
    }
    heap->elements[i] = last;
    return ret;
}

ElementType FindMin(const PriorityQueue heap)
{
    if (heap->size <= 0 || NULL == heap->elements) {
        return MinData;
    }
    return heap->elements[0];
}
