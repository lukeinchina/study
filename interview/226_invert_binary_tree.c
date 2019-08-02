#include <stdio.h>
#include <stdlib.h>

/**
 * Definition for a binary tree node.
 */

struct TreeNode {
    int val;
    struct TreeNode *left;
    struct TreeNode *right;
};


struct TreeNode* invertTree(struct TreeNode* root) {
    struct TreeNode *ptr = NULL;
    if (NULL != root) {
        ptr = root->right;
        root->right = invertTree(root->left);
        root->left  = invertTree(ptr);
    }
    return root;
}

struct TreeNode *insertTree(struct TreeNode *root, int val) {
    if (NULL == root) {
        root = (struct TreeNode *)malloc(sizeof(struct TreeNode));
        root->val = val;
        return root;
    }
    if (val < root->val) {
        root->left = insertTree(root->left, val);
    } else if (val > root->val) {
        root->right = insertTree(root->right, val);
    }
    return root;
}

void pre_order_traverse(struct TreeNode *root) {
    if (NULL != root) {
        printf("%d\t", root->val);
        pre_order_traverse(root->left);
        pre_order_traverse(root->right);
    }
}
void in_order_traverse(struct TreeNode *root) {
    if (NULL != root) {
        in_order_traverse(root->left);
        printf("%d\t", root->val);
        in_order_traverse(root->right);
    }
}


int
main(void) {
    int array[7] = {4, 2, 7, 1, 3, 6, 9};
    struct TreeNode *root = NULL;
    int i;
    for (i = 0; i < 7; i++) {
        root = insertTree(root, array[i]);
    }
    pre_order_traverse(root);
    printf("\n");
    root = invertTree(root);
    pre_order_traverse(root);
    printf("\n");
    return 0;
}
