#include <stdio.h>

bool 
containsDuplicate(int* nums, int numsSize) {
	int i, j;
	for (i = 0; i < numsSize; i++) {
		for (j = i+1; j < numsSize; j++) {
			if (nums[i] == nums[j]) {
				return true;
			}
		}
	}
	return false;
}

int
main(void) {
	int array [] = {1,3,4,2,6};
	printf("duplicate:%d\n", containsDuplicate(array, sizeof(array) / sizeof(array[0])));
	return 0;
}
