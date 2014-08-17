#pragma once

#include <stdlib.h>
#include "element_type.h"

struct TreeNode;
typedef struct TreeNode *Position;
typedef struct TreeNode *SearchTree;

SearchTree MakeEmpty(SearchTree root);

Position   Find(ElementType e, SearchTree root);
Position   FindMin(SearchTree root);
Position   FindMax(SearchTree root);

SearchTree Insert(ElementType e, SearchTree root);
SearchTree Delete(ElementType e, SearchTree root);

void PrintInOrder(SearchTree root);

/* Place in impliment file? */
struct TreeNode {
    ElementType data;
    SearchTree  left;
    SearchTree  right;
};
