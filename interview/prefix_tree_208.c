#include <stdio.h>
#include <stdlib.h>

#define ELEM_COUNT 26
typedef struct TrieNode{
	char c;
	bool end;
	struct TrieNode *next[ELEM_COUNT];
    
} Trie;

/** Initialize your data structure here. */
Trie* trieCreate() {
    return  (Trie *)calloc(1, sizeof(Trie));
}

/** Inserts a word into the trie. */
void trieInsert(Trie* obj, char* word) {
	Trie *parent = obj;
	while ('\0' != *word) {
		if (NULL == obj->next[*word-'a']) {
			obj->next[*word-'a'] = (Trie *)calloc(1, sizeof(Trie));
			obj->next[*word-'a']->c = *word;
		}
		parent = obj;
		obj    = obj->next[*word++ - 'a'];
	}
	parent->end = true;
}

/** Returns if the word is in the trie. */
bool trieSearch(Trie* obj, char* word) {
	Trie *parent = obj;
    while ('\0' != *word && NULL != obj->next[*word - 'a']) {
		parent = obj;
		obj    = obj->next[*word++ - 'a'];
	}
	return '\0' == *word && (parent->end == true);
} 

/** Returns if there is any word in the trie that starts with the given prefix. */
bool trieStartsWith(Trie* obj, char* prefix) {
    while ('\0' != *prefix && NULL != obj->next[*prefix - 'a']) {
		obj = obj->next[*prefix++ - 'a'];
	}
	return '\0' == *prefix;    
}

void trieFree(Trie* obj) {
	if (NULL == obj) {
		return ;
	}
	for (int i = 0; i < ELEM_COUNT; i++) {
		if (NULL != obj->next[i]) {
			trieFree(obj->next[i]);
		}
	}
	free(obj);
	return ;
}

int
main(void) {
	Trie *root = trieCreate();
	trieInsert(root, (char *)"apple");
    const char *test_search[5] = {"apple", "app", "applle", "appler", "test"};
	for (int i = 0; i < 5; i++) {
		printf("search(%s): %d\n", test_search[i], trieSearch(root, (char *)test_search[i]));
	}
	trieInsert(root, (char *)"testing");
    const char *test_start[5] = {"apple", "app", "applle", "appler", "test"};
	for (int i = 0; i < 5; i++) {
		printf("start(%s): %d\n", test_start[i], trieStartsWith(root, (char *)test_start[i]));
	}
	trieFree(root);
	return 0;
}
