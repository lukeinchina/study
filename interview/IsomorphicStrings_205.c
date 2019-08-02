#include <stdio.h>
#include <string.h>

bool isIsomorphic(const char* s, const char* t) {
    char map[128] = {0};
    char occ[128] = {0};
    memset(map, 0, sizeof(map));
    memset(occ, 0, sizeof(occ));
    for (; '\0' != *s && '\0' != *t; s++, t++) {
        if (map[*s] == '\0') {
            if (occ[*t]) {
                return false;
            }
            map[*s] = *t;
            occ[*t] = 1;
        } else if (map[*s] != *t) {
            return false;
        }
    }
    return '\0' == *s && '\0' == *t;
}

void test(const char *s, const char *t) {
    bool is = isIsomorphic(s, t);
    printf("[%s] [%s] is%s isomorphic string\n", s, t, is ? " " : "n't");
    return;
}

int
main(void) {
    test("egg", "add");
    test("title", "paper");
    test("foo", "bar");
    test("ab", "aa");
    test("aa", "ab");
}
