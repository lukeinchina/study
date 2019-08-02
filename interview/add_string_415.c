#include <string.h>
#include <stdlib.h>
#include <stdio.h>

char* addStrings(char* num1, char* num2) {
    int len1 = strlen(num1);
    int len2 = strlen(num2);
    char *str = (char *)malloc(4 + ((len1 > len2) ? len1 : len2));
    char *p = num1 + len1;
    char *q = num2 + len2;
    char *ptr = str;
    int  n, flag;

    flag = 0;
    for (; p > num1 && q > num2; --p, --q) {
        n = (*(q-1) - '0') + (*(p-1) - '0') + flag;
        flag = n / 10;
        n    = n % 10;
        *ptr++ = n + '0';
    }
    for (; p > num1; --p) {
        n = (*(p-1) - '0') + flag;
        flag = n / 10;
        n    = n % 10;
        *ptr++ = n + '0';
    }
    for (; q > num2; --q) {
        n = (*(q-1) - '0') + flag;
        flag = n / 10;
        n    = n % 10;
        *ptr++ = n + '0';
    }
    if (flag) {
        *ptr++ = '1';
    }
    *ptr-- = '\0';
    for (p = str; p < ptr; p++, ptr--) {
        /* swap */
        n = *p;
        *p = *ptr;
        *ptr = n;
    }
    return str;
}

int
main(void) {
    char str1[] = {"100000000000000000000000000"};
    char str2[] = {"9999"};
    printf("%s\n", addStrings(str1, str2));
    return 0;
}
