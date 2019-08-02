#include <stdio.h>

int maxSubArray(int* nums, int numsSize) {
    int i = 0;
    int max = -2147384647;
    int sum = 0;
    for (i = 0; i < numsSize; i++) {
        sum += nums[i];
        if (sum > max) {
            max = sum;
        }
        if (sum < 0) {
            sum = 0;
        }
    }
    return max;
}

int
main(int argc, char *argv[]) {
    // int datas[] = {-2,1,-3,4,-1,2,1,-5,4};
    int datas[] = {-2};
    int sum = maxSubArray(datas, sizeof(datas) / sizeof(datas[0]));;
    printf("%d\n", sum);
    return 0;
}
