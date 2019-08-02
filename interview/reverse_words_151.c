#include <stdio.h>
#include <string.h>
#include <ctype.h>

void reverse_string(char *begin, char *end) {
    char temp;
    for (; begin < end; begin++, end--) {
        temp = *end;
        *end = *begin;
        *begin = temp;
    }
    return ;
}
void reverseWords(char *s) {
    int len = strlen(s);
    char *p, *q;
    /* skip leading space */
    for (p = s; '\0' != *p && isspace(*p); p++) {
    }
    if ('\0' == *p) {
        *s = '\0';
        return;
    }

    /* trim trailing space */
    for (q = s + len - 1; q >= s && isspace(*q); q--) {
        *q = '\0';
    }

    /* trim leading space */
    if (p > s) {
        len = q - p + 1;
        memmove(s, p, len);
        *(s + len) = '\0';
    }

    /* reduce multiple spaces to a single space */
    p = q = s; 
    while ('\0' != *q) {
        while ('\0' != *q && !isspace(*q)) {
            *p++ = *q++;
        }
        if ('\0' == *q) {
            *p = *q;
        } else {
            *p++ = *q++;
        }
        /* skip redundannt space */
        while ('\0' != *q && isspace(*q)) {
            q++;
        }
    }

    len = p - s;
    reverse_string(s, s + len - 1);

    p = q = s;
    while ('\0' != *q) {
        p = q; 
        while('\0' != *q && !isspace(*q)) {
            ++q;
        }
        reverse_string(p, q-1);
        if ('\0' != *q) {
            ++q;
        }
    }

    return ;
}

int
main(void) {
    char test1 [] = {"        "};
    printf("[%s]\n", test1);
    reverseWords(test1);
    printf("[%s]\n", test1);
    return 0;
}
