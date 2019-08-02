#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* for exercise */
void 
sort_string(char *s) {
    int i, j;
    int len = strlen(s);
    char c, temp;
    for (i = 0; i < len; i++) {
        for (j = i+1; j < len; j++) {
            c = *(s+i);
            if (c > *(s+j)) {
                temp = c;
                c = *(s+j);
                *(s+j) = temp;
            }
            *(s+i) = c;
        }
    }
}



bool isAnagram(char* s, char* t) {
    int sdict[256] = {0};
    int tdict[256] = {0};
    for (; '\0' != *s; s++) {
        sdict[*s] = 1;
    }
    for (; '\0' != *t; t++) {
        tdict[*t] = 1;
    }
    return 0 == memcmp(sdict, sdict, sizeof(sdict));
}

int
main(int argc, char *argv[]) {
    char s1[] = "anagram";
    char s2[] = "nagaram";
    printf("%s:%s %d\n", s1, s2, isAnagram(s1, s2));
    return 0;
}
