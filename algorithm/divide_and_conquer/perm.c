/*
 *
 *
 *
 *
 *
 *
 */

#include <stdio.h>
#include <assert.h>

typedef int data_type;

#define swap(a, b) do {\
    data_type temp_elem = a; a = b; b = temp_elem;\
}while(0);


void perm(data_type *array, int begin, int end)
{
    if (begin >= end) {
        for (int i = 0; i <= end; i++) {
            printf("%d ", array[i]);
        }
        printf("\n");
        return;
    }

    for (int i = begin; i <= end; i++) {
        swap(array[begin], array[i]);
        perm(array, begin+1, end);
        swap(array[begin], array[i]);
    }
    return;
}

void perm_driver(data_type *array, int size)
{
    assert(NULL != array && size > 0);
    perm(array, 0, size - 1);
}

int
main(void)
{
    data_type  array[] = {1, 2, 3, 4};
    perm_driver(array, sizeof(array) / sizeof(array[0]));
    return 0;
}
