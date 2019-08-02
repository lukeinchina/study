#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
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
	while ('\0' != *word) {
		if (NULL == obj->next[*word-'a']) {
			obj->next[*word-'a'] = (Trie *)calloc(1, sizeof(Trie));
			obj->next[*word-'a']->c = *word;
		}
		obj    = obj->next[*word++ - 'a'];
	}
	obj->end = true;
}

/** Returns if the word is in the trie. */
bool trieSearch(Trie* obj, char* word) {
    while ('\0' != *word && NULL != obj->next[*word - 'a']) {
		obj    = obj->next[*word++ - 'a'];
	}
	return '\0' == *word && (obj->end == true);
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
main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("usage:%s operator_file case_file\n", argv[0]);
        exit(1);
    }
	Trie *root = trieCreate();

    int  op[40960];
    char line[256];
    FILE *op_fp = fopen(argv[1], "r");
    int  index = 0;
    assert(NULL != op_fp);
    while (NULL != fgets(line, sizeof(line), op_fp)) {
        if (strstr(line, "search") != NULL) {
            op[index] = 1;
        } else if (strstr(line, "insert") != NULL) {
            op[index] = 2;
        } else {
            printf("[%d] operation error :%s", index, line);
            op[index] = 3;
        }
        index += 1;
    }
    fclose(op_fp);

    index = 0;
    FILE *fp = fopen(argv[2], "r");
    assert(NULL != fp);
    for (; NULL != fgets(line, sizeof(line), op_fp); index++) {
        int len = strlen(line);
        while (isspace(line[len-1])) {
            line[--len] = '\0';
        }
        bool succ = false;
        if (op[index] == 2) {
            trieInsert(root, line);
            printf("null\n");
            continue;
        }
        succ = trieSearch(root, line);
        if (succ) {
            printf("true\n");
        } else {
            printf("false\n");
        }
    }
    fclose(fp);
	return 0;
}
