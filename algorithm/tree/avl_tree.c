/*
 * =====================================================================================
 *
 *       Filename:  avl_tree.c
 *
 *    Description:  study avl tree algorithm.
 *
 *        Version:  1.0
 *        Created:  05/27/2014 04:33:28 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  luke, lukeinchina@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <algorithm>
#include "avl_tree.h"

/*
 * leaves node's height is 0;
 *
 */
static int
Height(Position t)
{
    if (NULL == t) {
        return -1;
    }else {
        return t->height;
    }
}


/*
 *                  K1                              K2
 *                 /  \                            /  \
 *                /    \                          /    \
 *              K2     sss    -------->          K3     K1
 *              /  \                                   /  \
 *             /    \                                 /    \
 *            K3    ooo                             ooo    sss
 *
 */
static Position
SingleRotateWithLeft(Position k1)
{
    Position k2 = k1->left;;
    k1->left    = k2->right;
    k2->right   = k1;

    k1->height = std::max(Height(k1->right), Height(k1->left)) + 1;
    k2->height = std::max(Height(k2->right), Height(k2->left)) + 1;
    return k2;
}


/*
 *                  K1                              K2
 *                 /  \                            /  \
 *                /    \                          /    \
 *              ooo    K2    -------->          K1     K3 
 *                    /  \                      / \       
 *                   /    \                    /   \       
 *                 sss    K3                 ooo   sss       
 *
 */
static Position
SingleRotateWithRight(Position k1)
{
    Position k2 = k1->right;
    k1->right   = k2->left;
    k2->left    = k1;
    k1->height = std::max(Height(k1->left), Height(k1->right)) + 1;
    k2->height = std::max(Height(k2->left), Height(k2->right)) + 1;
    return k2;
}

static Position
DoubleRotateWithLeft(Position root)
{
    root->left = SingleRotateWithRight(root->left);
    return SingleRotateWithLeft(root);
}

static Position
DoubleRotateWithRight(Position root)
{
    root->right = SingleRotateWithLeft(root->right);
    return SingleRotateWithRight(root);
}

AvlTree
Insert(ElementType e, AvlTree root)
{
    if (NULL == root) {
        root = (struct AvlNode*)malloc(sizeof(struct AvlNode)); 
        root->left   = root->right = NULL;
        root->height = 0;
        root->data   = e;
        return root;
    } else if (e < root->data) {
        root->left = Insert(e, root->left);
        if (Height(root->left) > Height(root->right) + 1) {
            // if (Height(root->left->left) > Height(root->left->right)) 
            if (e < root->left->data) {
                root = SingleRotateWithLeft(root);
            } else {
                root = DoubleRotateWithLeft(root);
            }
        }
    } else if (e > root->data) {
        root->right = Insert(e, root->right);
        if (Height(root->right) > Height(root->left) + 1) {
            // if (Height(root->right->right) > Height(root->right->left)) 
            if (e > root->right->data) {
                root = SingleRotateWithRight(root);
            } else {
                root = DoubleRotateWithRight(root);
            }
        }
    }
    // exist. nothing to do

    root->height = std::max(Height(root->right), Height(root->left)) + 1;
    return root;
}

Position FindMax(const AvlTree root)
{
    if (NULL == root) {
        return NULL;
    }
    Position t = root;
    while (NULL != t->right) {
        t = t->right;
    }
    return t;
}

Position FindMin(const AvlTree root)
{
    if (NULL == root) {
        return NULL;
    }
    Position t = root;
    while (NULL != t->left) {
        t = t->left;
    }
    return t;
}

AvlTree
Delete(ElementType e, AvlTree root)
{
    if (NULL == root) {
        return NULL;
    }

    if (e < root->data) {
        root->left = Delete(e, root->left);
    } else if (e > root->data) {
        root->right = Delete(e, root->right);
    } else {
        // find.
        if (NULL != root->left && NULL != root->right) {
            Position max = FindMax(root->left);
            root->data = max->data;
            root->left = Delete(max->data, root->left);
        } else if (NULL == root->left && NULL == root->right){
            free(root);
            return NULL;
        }else {
            Position t = root;
            root = (NULL != root->left) ? root->left : root->right;
            free(t);
            return root;
        }
    }

    if (Height(root->left) > Height(root->right) + 1) {
        if (Height(root->left->left) > Height(root->left->right)) {
            return SingleRotateWithLeft(root);
        } else {
            return DoubleRotateWithLeft(root);
        }
    } else if (Height(root->right) > Height(root->left) + 1) {
        if (Height(root->right->right) > Height(root->right->left)) {
            return SingleRotateWithRight(root);
        } else {
            return DoubleRotateWithRight(root);
        }
    }

    root->height = std::max(Height(root->right), Height(root->left)) + 1;
    return root;
}

void PrintInOrder(const AvlTree root)
{
    if (NULL == root) {
        return ;
    }
    PrintInOrder(root->left);
    PrintElement(root->data);
    PrintInOrder(root->right);
}

/*--------------------------------for test-----------------------------------*/

void PrintWithHeightInOrder(const AvlTree root)
{
    if (NULL == root) {
        return ;
    }
    PrintWithHeightInOrder(root->left);
    // PrintElement(root->data);
    printf("%d(%d)\t", root->data, root->height);
    PrintWithHeightInOrder(root->right);
}

bool check_balance(const AvlTree root)
{
    if (NULL == root) {
        return true;
    }
    if (Height(root->left) > Height(root->right)+1 || 
            Height(root->right) > Height(root->left)+1) {
        printf("not balance:%d{%d}(%d,%d)\n", root->data, root->height, 
                Height(root->left), Height(root->right));
        return false;
    }
    if (!check_balance(root->left)) {
        return false;
    } else if (!check_balance(root->right)) {
        return false;
    }
    return true;
}

