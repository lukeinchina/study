#include <stdlib.h>
/**
 * Definition for a binary tree node.
 */
struct TreeNode {
    int val;
    struct TreeNode *left;
    struct TreeNode *right;
};

bool isValidBST(struct TreeNode* root) {
    struct TreeNode *min = NULL;
    struct TreeNode *max = NULL;
    if (NULL == root || (NULL == root->left && NULL == root->right)) {
        return true;
    }
    if (!isValidBST(root->left) || !isValidBST(root->right)) {
        return false;
    }
    if (NULL != root->left) {
        max = root->left;
        while (NULL != max->right) {
            max = max->right;
        }
    } 
    if (NULL != root->right) {
        min = root->right;
        while (NULL != min->right) {
            min = min->right;
        }
    }
    if (NULL != max && max->val >= root->val) {
        return false;
    }
    if (NULL != min && min->val <= root->val) {
        return false;
    }
    return true;
}

int
main(void) {
    return 0;
}
