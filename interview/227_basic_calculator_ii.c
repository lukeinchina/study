#include <stdio.h>
#include <ctype.h>

int calculate_op(int a, int b, char op) {
    switch (op) {
    case '+':
        return a + b;
    case '-':
        return a - b;
    case '*':
        return a * b;
    case '/':
        return a / b;
    default:
        return 0;
    }
}

int calculate(char* s) {
    int result = 0;
    int base = 0;
    int nums[8];
    char ops[8];
    int levels[256] = {0};
    int i, k;
    levels['+'] = 1;
    levels['-'] = 1;
    levels['*'] = 2;
    levels['/'] = 2;

    for (i = k = 0; ; s++) {
        if ('\0' == *s) {
            nums[i++] = base;
            while (k > 0) {
                result = calculate_op(nums[i-2], nums[i-1], ops[--k]);
                i -= 2;
                nums[i++] = result;
            }
            return nums[0];
        } else if (isspace(*s)) {
            continue;
        } else if (isdigit(*s)) {
            base = base * 10 + *s - '0';
        } else {
			nums[i++] = base;
			while (k > 0 && levels[*s] <= levels[ops[k-1]]) {
                result = calculate_op(nums[i-2], nums[i-1], ops[--k]);
				i -= 2;
                nums[i++] = result;
            }
			ops[k++] = *s;
            base = 0;
        }
    }
    return  result;
}

int
main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("usage:%s a+b*c/d\n", argv[0]);
        return 1;
    }
    printf("result:%d for %s\n", calculate(argv[1]), argv[1]);
    return 0;
}
