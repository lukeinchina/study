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


void kth_in_order(struct TreeNode *root, int k, int *count, int *dst) {
    if (NULL == root) {
        return ;
    }

    if (NULL != root->left) {
        kth_in_order(root->left, k, count, dst);
    }
    *count += 1;
    if (*count == k) {
        *dst = root->val;
        return ;
    }
    if (NULL != root->right) {
        kth_in_order(root->right, k, count, dst);
    }
    return ;
}

int kthSmallest(struct TreeNode* root, int k){
    int count = 0;
    int dst = 0;
    kth_in_order(root, k, &count, &dst);
    return dst;
}

int
main(int argc, char *argv[]) {
    int k = 0;
    int array[7] = {5, 12, 7, 1, 3, 6, 9};
    struct TreeNode *root = NULL;
    int i;
    if (argc < 2) {
        printf("usage:%s n\n", argv[0]);
        return 1;
    }
    k = atoi(argv[1]);
    for (i = 0; i < 7; i++) {
        root = insertTree(root, array[i]);
    }
    pre_order_traverse(root);
    printf("\n");
    // root = invertTree(root);
    in_order_traverse(root);
    printf("\n");
    printf("%d'th smallest:%d\n", k, kthSmallest(root, k));
    return 0;
}
