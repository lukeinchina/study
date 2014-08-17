/*
 * =====================================================================================
 *
 *       Filename:  ac_tire.c
 *
 *    Description:  Aho-Corasick automation
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
#include <queue>
#include <iostream>

#include "ac_tire.h"
struct ACTireNode {
    char c;
    char flag; /* a word */
    struct ACTireNode *next[MAX_CHILDREN];
    struct ACTireNode *fail;
};

ACTireTree BuildAcAutomation(ACTireTree root)
{
    if (NULL == root) {
        return NULL;
    }
    struct ACTireNode *parent = NULL;
    struct ACTireNode *curr = NULL;
    std::queue<ACTireNode *> my_queue;
    int i = 0;
    for (i = 0; i < MAX_CHILDREN; i++) {
        if (NULL != root->next[i]) {
            root->next[i]->fail = root;
            my_queue.push(root->next[i]);
        }
    }
    while (!my_queue.empty()) {
        parent = my_queue.front();
        my_queue.pop();
        for (i = 0; i < MAX_CHILDREN; i++) {
            if (NULL == parent->next[i]) {
                continue;
            }
            curr = parent->next[i];
            for (; NULL != parent->fail; parent = parent->fail) {
                if (NULL != parent->fail->next[i]) {
                    curr->fail = parent->fail->next[i];
                    break;
                }
            }
            /* until the root */
            if (NULL == parent->fail) {
                curr->fail = root;
            }
        }
    }
    return root;
}

ACTireTree DestoryTire(ACTireTree root)
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
ACTireTree InsertTire(ACTireTree root, const char *str)
{
    struct ACTireNode *ptr = NULL;
    if (NULL == str) {
        return NULL;
    }
    if (NULL == root) {
        root = (ACTireTree)malloc(sizeof(struct ACTireNode));
        if (NULL == root) {
            return NULL;
        }
        memset(root, 0, sizeof(struct ACTireNode));
    }
    ptr = root;
    for(; '\0' != *str; ++str) {
        if ('a' > *str || 'z' < *str) {
            continue;
        }
        /**/
        if (NULL == ptr->next[*str - 'a']) {
            ptr->next[*str - 'a'] = (ACTireTree)malloc(sizeof(struct ACTireNode));
            ptr = ptr->next[*str - 'a'];
            memset(ptr, 0, sizeof(struct ACTireNode));
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
static void PrintWord(ACTireTree root, char *buff, int top)
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
}

/*
 * test
 */
void PrintAllWord(ACTireTree root) {
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
