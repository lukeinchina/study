#include <stdio.h>

bool containsNearbyAlmostDuplicate(int* nums, int numsSize, int k, int t) {
	int i, j, val;
	for (i = k; i > 0; i--) {
		for (j = 0; j < numsSize; j++) {
			val = nums[i] - nums[i+k] > 0 ? nums[i] - nums[i+k] : nums[i+k] - nums[i];
			if (val <= t) {
				return true;
			}
		}
	}
	return false;
}

int
main(int argc, char *argv[]) {
	int test1[] = {1,2,3,1};
	int test2[] = {1,0,1,1};
	int test3[] = {1,5,9,1,5,9};
	bool (*funcp)(int *, int, int, int);
	funcp =  containsNearbyAlmostDuplicate;
    printf("%d\n", (*funcp)(test1, 4, 3, 0));
    printf("%d\n", (*funcp)(test2, 4, 1, 2));
    printf("%d\n", (*funcp)(test3, 5, 2, 3));
	return 0;
}
