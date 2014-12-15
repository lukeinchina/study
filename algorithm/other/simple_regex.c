#include <stdio.h>
#include <assert.h>

int match(const char *text, const char *regex);
int match_star(char c, const char *text, const char *regex);

/*
 * @brief: match * char
 *
 * @return : 1 : match; 0 : do not match
 *
 */
int match_star(char c, const char *text, const char *regex)
{
    do {
        if(match(text, regex)) {
            return 1;
        }
    }while('\0' != *text && (*text++ == c));
    return 0;
}

/*
 * @brief: regex match 
 *
 * @return : 1 : match; 0 : do not match
 *
 */
int match(const char *text, const char *regex)
{
    /* match and over */
    if ('\0' == *regex) {
        return 1;
    }
    if (*(regex+1) == '*') {
        if (match_star(*regex, text, regex+2)) {
            return 1;
        }
    }else if (*regex == *text) {
        return match(text+1, regex+1);
    }
    return 0;
}

/*
 * @brief: driver function for regex match 
 *
 * @return : 1 : match; 0 : do not match
 *
 */
int match_driver(const char *text, const char *regex)
{
    /* check whether the input regex is valid */

    do {
        if (match(text, regex)) {
            return 1;
        }
    }while('\0' != *text++);

    return 0;
}

int
main(void) {
    const char *text[] = {"dagabcdefg",
        "dagabcdefg",
        "dagabcdefg",
        "google"
    };
    const char *regex[] = {"abcde",
        "g*abc",
        "g*ag*",
        "o*d"
    };
    for (int i = 0; i < sizeof(text)/sizeof(text[0]); i++) {
        if (match_driver(text[i], regex[i])) {
            printf("[%d] [%s] does match [%s]\n", i+1, text[i], regex[i]);
        }else{
            printf("[%d] [%s] does not match [%s]\n", i+1, text[i], regex[i]);
        }
    }
    return 0;
}
