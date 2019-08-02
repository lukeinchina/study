#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

int
is_palindromic(const char *begin, const char *end) {
    assert(begin <= end);
    for (; begin < end; begin++, end--) {
        if (*begin != *end) {
            return 0;
        }
    }
    return 1;
}

int countSubstrings(const char* s) {
    const char *q, *p = s;
    int res, size = strlen(s) + 1;
    int *nums = (int *)malloc(size * sizeof(int));
    nums[0] = 0;
    for (; '\0' != *p; p++) {
        nums[p - s + 1] = 1 + nums[p - s];
        for (q = s; q < p; q++) {
            if (*q == *p && is_palindromic(q, p)) {
                nums[p - s + 1] += 1;
            }
        }
    }
    res = nums[size-1];
    free(nums);
    return res;
}

int
main(int argc, char *argv[]) {
    int i = 0;
    const char *cases[] = {
        "",
        "abc",
        "aaa",
        "aba",
        "abcba"
    };
    for (i = 0; i < sizeof(cases) / sizeof(cases[i]); i++) {
        printf("[%s] : %d\n",  cases[i], countSubstrings(cases[i]));
    }
    return 0;
}
