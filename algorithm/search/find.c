#include <stdio.h>

/*
 *  array [in]: sorted array by insc
 *
 *
 */
int first_find(int array[], int size, int x)
{
    int left = 0;
    int right = size - 1;
    int first = -1;
    int mid = 0;
    while (left <= right) {
        mid = (left + right) / 2;
        if (array[mid] < x) {
            left = mid + 1;
        }else if (array[mid] > x) {
            right = mid - 1;
        }else {
            first = mid;
            right = mid - 1;
        }
    }
    return first;
}

/*
 *  array [in]: sorted array by insc
 *
 *
 */
int last_find(int array[], int size, int x)
{
    int left = 0;
    int right = size - 1;
    int last = -1;
    int mid = 0;
    while (left <= right) {
        mid = (left + right) / 2;
        if (array[mid] < x) {
            left = mid + 1;
        }else if (array[mid] > x) {
            right = mid - 1;
        }else {
            last = mid;
            left = mid + 1;
        }
    }
    return last;
}

int
main(void)
{
    int array[] = {1, 3, 5, 7, 9, 9, 9, 15,17,19};
    printf("%d\n", first_find(array, 10, 9));
    printf("%d\n", last_find(array, 10, 9));
    return 0;
}
