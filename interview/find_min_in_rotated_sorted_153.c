#include <stdio.h>
#include <limits.h>

int findMin(int* nums, int numsSize) {
    int i;
    for (i = 1; i < numsSize; i++) {
        if (nums[i] < nums[i-1]) {
            break;
        }
    }
    if (i >= numsSize) {
        return nums[0];
    }
    return nums[0] > nums[i] ? nums[i] : nums[0];
}

int
main(void) {
    int test1[] = {4,5,6,7,0,1,2};
    int test2[] = {3,4,5,1,2};
    int test3[] = {2, 3,4,5,7,9};
    printf("[3,4,5,1,2], min:%d\n", findMin(test2, 5));
    printf("[4,5,6,7,0,1,2], min:%d\n", findMin(test1, 7));
    printf("[],min:%d\n", findMin(test3, 6));
    return 0;
}
