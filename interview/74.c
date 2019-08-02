#include <stdio.h>

void sortColors(int* nums, int numsSize) {
    int idx,i,left, right, swp;
    idx = i = 0;
    left = right = 0;
    for (idx = 0; idx < 2; idx++) {
        while (left < numsSize && nums[left] == idx) left++;
        right = left + 1;
        while (right < numsSize) {
            while (right < numsSize && nums[right] != idx) right++;
            if (right >= numsSize) {
                break;
            }
            swp = nums[left];
            nums[left] = nums[right];
            nums[right] = swp;
            left += 1;
        }
    }
}

void print_array(const int *array, int size) {
    int i = 0;
    for (; i < size; i++) {
        printf("%d,", array[i]);
    }
    printf("\n");
}

int
main(int argc, char *argv[]) {
    int nums[] = {2, 1};//, 1, 2, 0, 0, 0, 2, 1, 0, 2, 2, 2}; 
    print_array(nums, sizeof(nums) / sizeof(nums[0]));
    sortColors(nums, sizeof(nums) / sizeof(nums[0]));
    print_array(nums, sizeof(nums) / sizeof(nums[0]));
    return 0;
}
