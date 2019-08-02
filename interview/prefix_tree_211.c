#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define ELEM_COUNT 26
typedef struct WordNode{
    char c;
    char flag;
    struct WordNode *nexts[ELEM_COUNT];
} WordDictionary;

/** Initialize your data structure here. */
WordDictionary* wordDictionaryCreate() {
    return (WordDictionary *)calloc(1, sizeof(WordDictionary));
}

/** Adds a word into the data structure. */
void wordDictionaryAddWord(WordDictionary* obj, const char* word) {
    for (; '\0' != *word; word++) {
        if (NULL == obj->nexts[*word - 'a']) {
            obj->nexts[*word - 'a']    = (WordDictionary *)calloc(1, sizeof(WordDictionary));
            obj->nexts[*word - 'a']->c = *word;
        }
        obj = obj->nexts[*word - 'a'];
    }
    obj->flag = 1;
}

/** Returns if the word is in the data structure. A word could contain the dot character '.' to represent any one letter. */
bool wordDictionarySearch(WordDictionary* obj, const char* word) {
    for (; '\0' != *word; word++) {
        if ('.' == *word) {
            for (int i =0; i < ELEM_COUNT;i++) {
                if (NULL != obj->nexts[i] && wordDictionarySearch(obj->nexts[i], word+1)) {
                    return true;
                }
            }
            return false;
        } else if (NULL == obj->nexts[*word - 'a']) {
            return false;
        }
        obj = obj->nexts[*word - 'a'];
    }
    return obj->flag == 1;
}

void wordDictionaryFree(WordDictionary* obj) {
    int i;
    for (i = 0; i < ELEM_COUNT; i++) {
        if (NULL != obj->nexts[i] ) {
            wordDictionaryFree(obj->nexts[i]);
        }
    }
    free(obj);
    return;
}

/**
 * Your WordDictionary struct will be instantiated and called as such:
 * struct WordDictionary* obj = wordDictionaryCreate();
 * wordDictionaryAddWord(obj, word);
 * bool param_2 = wordDictionarySearch(obj, word);
 * wordDictionaryFree(obj);
 */

int
main(int argc, char *argv[]) {
    size_t i = 0;
    WordDictionary* obj = wordDictionaryCreate();
    wordDictionaryAddWord(obj, "bad");
    wordDictionaryAddWord(obj, "mad");
    wordDictionaryAddWord(obj, "dad");
    const char *search_words[] = {"pad", "bad", ".ad", "b..", "", NULL};
    for (i = 0; NULL != search_words[i]; i++) {
        printf("%s: %s\n", search_words[i], wordDictionarySearch(obj, search_words[i]) ? "true" : "false");
    }
    wordDictionaryFree(obj);
    return 0;
}
