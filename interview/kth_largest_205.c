#include <stdio.h>

int findKthLargest(int* nums, int numsSize, int k) {
    int left, right, pivot, temp;
    if (numsSize < 2) {
        return nums[0];
    }

    pivot = nums[numsSize / 2];
    for (left = 0, right = numsSize - 1; left <= right; left++, right--) {
        while (left <= right && nums[left] > pivot) {
            left++;
        }
        while (left <= right && nums[right] < pivot) {
            right--;
        }
        if (left >= right) {
            break;
        }
        temp = nums[left];
        nums[left] = nums[right];
        nums[right] = temp;
    }

    if (k <= left) {
        return findKthLargest(nums, left, k);
    }else {
        return findKthLargest(nums + left, numsSize - left, k - left);
    }
}

int
main(void) {
    int i = 0;
    int test1[] = {3,2,1,5,6,4};
    int test2[] = {3,2,3,1,2,4,5,5,6};
    printf("test1, 2th largest element is :%d\n", findKthLargest(test1, 6, 2));
    printf("test2, 4th largest element is :%d\n", findKthLargest(test2, 9, 4));
   
    for (i = 0; i < 9; i++) {
        printf("%d\t", test2[i]);
    }
    printf("\n");
    return 0;
}
