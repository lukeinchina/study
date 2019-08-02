#include <stdio.h>

void merge(int* nums1, int m, int* nums2, int n) {
    int *dst = nums1 + m + n - 1;
    int *p1 = nums1 + m - 1;
    int *p2 = nums2 + n - 1;
    while (p1 >= nums1 && p2 >= nums2) {
        *dst-- = *p1 > *p2 ? *p1-- : *p2--;
    }
    while (p1 >= nums1) {
        *dst-- = *p1--;
    }
    while (p2 >= nums2) {
        *dst-- = *p2--;
    }
    return;
}

int
main(int argc, char *argv[]) {
    int i = 0;
    int nums1[8] = {-3,-2,1,4};
    int nums2[4] = {-2,-1,2,5};
    merge(nums1, 4, nums2, 4);
    for (i = 0; i < 8; i++) {
        printf("%d ", nums1[i]);
    }
    printf("\n");
    return 0;
}
