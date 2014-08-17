#ifndef _binary_heap_h_
#define _binary_heap_h_

typedef int ElementType;

struct HeapStruct;
typedef struct HeapStruct *PriorityQueue;

PriorityQueue Initialize(int MaxElements);
void Destory(PriorityQueue heap);
void MakeEmpty(PriorityQueue heap);
int Insert(PriorityQueue heap, ElementType x);
ElementType DeleteMin(PriorityQueue heap);
ElementType FindMin(PriorityQueue heap);
int IsEmpty(PriorityQueue heap);
int IsFull(PriorityQueue heap);

#endif
