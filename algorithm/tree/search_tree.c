/*
 * =====================================================================================
 *
 *       Filename:  search_tree.c
 *
 *    Description:  studing search tree algorithm. 
 *
 *        Version:  1.0
 *        Created:  05/27/2014 03:35:28 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  luke, lukeinchina@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "search_tree.h"

SearchTree MakeEmpty(SearchTree root)
{
    if (NULL != root) {
        MakeEmpty(root->left);
        MakeEmpty(root->right);
        free(root);
    }
    return NULL;
}

Position FindRecursive(ElementType e, SearchTree root)
{
    if (NULL == root) {
        return NULL;
    }
    if (e < root->data) {
        return FindRecursive(e, root->left);
    }else if (e > root->data) {
        return FindRecursive(e, root->right);
    }else {
        return root;
    }
}

Position Find(ElementType e, SearchTree root)
{
    while (NULL != root) {
        if (e < root->data) {
            root = root->left;
        }else if (e > root->data) {
            root = root->right;
        }else { /*  find it */
            break;
        }
    }
    return root;
}

Position FindMin(SearchTree root)
{
    if (NULL == root) {
        return NULL;
    }
    while (NULL != root->left) {
        root = root->left;
    }
    return root;
}

Position FindMax(SearchTree root)
{
    if (NULL == root) {
        return NULL;
    }
    while (NULL != root->right) {
        root = root->right;
    }
    return root;
}

SearchTree Insert(ElementType e, SearchTree root)
{
    if (NULL == root) {
        root = (struct TreeNode *)malloc(sizeof(struct TreeNode));
        root->data = e;
        root->left = root->right = NULL;
        return root;
    }

    if (e < root->data) {
        root->left  = Insert(e, root->left);
    }else if (e > root->data) {
        root->right = Insert(e, root->right);
    }
    /* already exist . nothing to do */

    return root;
}

SearchTree Delete(ElementType e, SearchTree root)
{
    if (NULL == root) {
        return NULL; // error
    }

    if (e < root->data) {
        root->left = Delete(e, root->left);
    }else if (e > root->data) {
        root->right = Delete(e, root->right);
    }else {
        if (NULL != root->left && NULL != root->right) {
            root->data = FindMin(root->right)->data;
            root->right = Delete(root->data, root->right);
        }else {

            Position t = root;
            /* also handles 0's children, in this situation, root == NULL !!! */
            if (NULL == root->left) {
                root = root->right;
            }else if (NULL == root->right) {
                root = root->left;
            }
            free(t);
        }
    }
    return root;
}

void PrintInOrder(SearchTree root)
{
    if (NULL == root) {
        return ;
    }
    PrintInOrder(root->left);
    PrintElement(root->data);
    PrintInOrder(root->right);
}
