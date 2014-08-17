/*
 * =====================================================================================
 *
 *       Filename:  tire.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/14/2014 10:11:34 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  luke (), lukeinchina@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tire.h"
struct TireNode {
    char c;
    char flag; /* a word */
    struct TireNode *next[MAX_CHILDREN];
};

TireTree DestoryTire(TireTree root)
{
    int i = 0;
    if (NULL == root) {
        return NULL;
    }
    
    for (i = 0; i < MAX_CHILDREN; i++) {
        if (NULL != root->next[i]) {
            root->next[i] = DestoryTire(root->next[i]);
        }
    }
    free(root);
    return NULL;
}

/* --------------------------------------------------------------------- */
/*
 * @brief: str must be lower case.
 *
 *
 */
TireTree InsertTire(TireTree root, const char *str)
{
    struct TireNode *ptr = NULL;
    if (NULL == str) {
        return NULL;
    }
    if (NULL == root) {
        root = (TireTree)malloc(sizeof(struct TireNode));
        if (NULL == root) {
            return NULL;
        }
        memset(root, 0, sizeof(struct TireNode));
    }
    ptr = root;
    for(; '\0' != *str; ++str) {
        if ('a' > *str || 'z' < *str) {
            continue;
        }
        /**/
        if (NULL == ptr->next[*str - 'a']) {
            ptr->next[*str - 'a'] = (TireTree)malloc(sizeof(struct TireNode));
            ptr = ptr->next[*str - 'a'];
            memset(ptr, 0, sizeof(struct TireNode));
            ptr->c = *str;
        }else {
            ptr = ptr->next[*str - 'a'];
        }
    }
    if ('\0' == *str) {
        ptr->flag = 1;
    }
    return root;
}


/*
 * test.
 */
static void PrintWord(TireTree root, char *buff, int top)
{
    int i = 0;
    int leaf = 1;
    if (NULL == root) {
        return;
    }
    /* a word completely */
    if (root->flag) {
        buff[top] = root->c;
        buff[top+1] = '\0';
        printf("%s\n", buff);
    }
    for (i = 0; i < MAX_CHILDREN; i++) {
        if (NULL != root->next[i]) {
            buff[top] = root->c;
            PrintWord(root->next[i], buff, top+1);
            leaf = 0;
        }
    }
    /*
    if (leaf) {
        buff[top++] = root->c;
        buff[top] = '\0';
        printf("%s\n", buff);
    }  */
}

/*
 * test
 */
void PrintAllWord(TireTree root) {
    int i = 0;
    char buff[1024];
    if (NULL == root) {
        return;
    }
    for (i = 0; i < MAX_CHILDREN; i++) {
        if (NULL != root->next[i]) {
            PrintWord(root->next[i], buff, 0);        
        }
    }
}
