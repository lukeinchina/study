#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

/*---------------------------------------------------------*/
/* binary search tree struture */
struct TreeNode {
    int              data;
    struct TreeNode *left;
    struct TreeNode *right;
};

typedef struct TreeNode *Tree;
typedef struct TreeNode *List;


/* middle struct */
struct HeadTailPair{
    struct TreeNode *head;
    struct TreeNode *tail;
};

/*---------------------------------------------------------*/
/* convert function */
struct HeadTailPair
tree_to_list(Tree root) {
    struct HeadTailPair current = {NULL, NULL};
    if (NULL == root) {
        return current;
    }
    struct HeadTailPair left  = tree_to_list(root->left);
    struct HeadTailPair right = tree_to_list(root->right);
    root->left  = left.tail;
    root->right = right.head;

    if (NULL == left.tail) {
        left.head = left.tail = root;
    } else {
        left.tail->right = root;
    }

    if (NULL == right.head) {
        right.head = right.tail = root;
    } else {
        right.head->left = root;
    }
    current.head = left.head;
    current.tail = right.tail;
    return current;
}

List
tree_to_list_driver(Tree root) {
    struct HeadTailPair temp;
    temp = tree_to_list(root);
    return temp.head; // first element
}

void PrintList(List head) {
    while (NULL != head) {
        printf("%d\t", head->data);
        head = head->right;
    }
    printf("\n");
}

/*---------------------------------------------------------*/

/*---------------------------------------------------------*/
/* about binary tree */
struct TreeNode *
binary_search_tree_insert(struct TreeNode *root, int data) {
    if (NULL == root) {
        root = (struct TreeNode *)malloc(sizeof(struct TreeNode));
        assert(NULL != root);
        root->data  = data;
        root->left  = NULL;
        root->right = NULL;
    } else {
        if (data < root->data) {
            root->left = binary_search_tree_insert(root->left, data);
        } else if (data > root->data) {
            root->right = binary_search_tree_insert(root->right, data);
        } else {
            printf("Node[%d] does exist. Can not be inserted again\n", data);
        }
    }
    return root;
}

struct TreeNode *
create_binary_search_tree(int node_count) {
    int i     = 0;
    struct TreeNode * root = NULL;
    if (node_count < 1 || node_count > 100) {
        printf("tree size should be [1,100]\n");
        return NULL;
    }

    srand((unsigned)time(NULL));
    for (i = 0; i < node_count; i++) {
        root = binary_search_tree_insert(root, rand() % 100);
    }
    return root;
}

struct TreeNode *
destory_binary_search_tree(struct TreeNode *root) {
    if (NULL != root) {
        root->left  = destory_binary_search_tree(root->left);
        root->right = destory_binary_search_tree(root->right);
        free(root);
    }
    return NULL;
}

void 
print_binary_search_tree(const struct TreeNode *root) {
    if (NULL == root) {
        return;
    }
    print_binary_search_tree(root->left);
    printf("%d\t", root->data);
    print_binary_search_tree(root->right);
}

int
main(void) {
    struct TreeNode *root = create_binary_search_tree(10);
    print_binary_search_tree(root);
    printf("\n");
    // destory_binary_search_tree(root);
    List head = tree_to_list_driver(root);
    PrintList(head);
    return 0;
}
