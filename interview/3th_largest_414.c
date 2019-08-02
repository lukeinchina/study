#include <stdio.h>
#include <limits.h>

int thirdMax(int* nums, int numsSize) {
    int a,b,c,i;
    int count = 0;
    a = b = c = INT_MIN;
    for (i = 0; i < numsSize; i++) {
        if (nums[i] > a) {
            c = b;
            b = a;
            a = nums[i];
            count++;
        } else if (nums[i] > b && nums[i] != a) {
            c = b;
            b = nums[i];
            count++;
        } else if (nums[i] >= c && nums[i] != b && nums[i] != a) {
            c = nums[i];
            count++;
        }
    }
    return  count > 2 ? c : a;
}

int
main(void) {
    int test1[] = {1,-2147483648,2};
    printf("%d\n", thirdMax(test1, 2));
    return 0;
}
