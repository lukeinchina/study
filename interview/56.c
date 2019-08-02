#include <stdio.h>

/**
 * Definition for an interval.
 */
struct Interval {
    int start;
    int end;
};

struct Interval* 
merge(struct Interval* intervals, int intervalsSize, int* returnSize) {
    int i = 0;
    struct Interval *dst = (struct Interval *)malloc(sizeof(struct Interval) * intervalsSize);
    *returnSize = 0;
    struct Interval one = {0,0};
    while (i < intervalsSize) {
        /* */
        one = intervals[i];
        for (i += 1; i < intervalsSize && one.end >= intervals[i].start; i++) {
        }
    }
}

int
main(int argc, char *argv[]) {
    struct Interval invls[] = {{1,3},{2,6},{8,10},{15,18}};
    return 0;
}
