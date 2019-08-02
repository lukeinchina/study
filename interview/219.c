#include <stdio.h>

bool containsNearbyDuplicate(int* nums, int numsSize, int k) {
   int i, j;
   for (i = 0; i < numsSize; i++) {
	   for (j = i+1; j < numsSize; j++) {
		   if (nums[i] == nums[j]) {
			   if (j - i <= k) {
				   return true;
			   }
			   break;
		   }
	   }
   }
   return false;
}

int
main(void) {
	int array[] = {1,3,4,2,6};
	bool flag = containsNearByDuplicate(array, 5, 4);
	return 0;
}
