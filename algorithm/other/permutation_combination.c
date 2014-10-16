#include <stdio.h>

#define swap(a,b) do {temp=a;a=b;b=temp;}while(0)

#define print_result() do { \
        for (int j = 0; j <= end; j++) { printf("%c", A[j]); } \
        printf("\n");\
} while(0)

void full_permutation(char *A, int begin, int end)
{
    int i = 0; 
    char temp = 0;
    if (begin >= end) {
        print_result();
        return;
    }
    for (i = begin; i <= end; i++) {
        swap(A[begin], A[i]);
        full_permutation(A, begin+1, end);
    }
    return ;
}

void combination()
{
    return;
}

int
main(void) {
    char A[5] = {'A', 'B', 'C', 'D', 'D'};
    full_permutation(A, 0, 4);
    return 0;
}
