#pragma once

#include "element_type.h"
/* AVL means: Adelson Velskii and Landis */


/*--------------  begin ---------------*/
struct AvlNode;
typedef struct AvlNode *AvlTree;
typedef struct AvlNode *Position;

AvlTree MakeEmpty(AvlTree root);
Position Find(const ElementType e, const AvlTree root);
Position FindMax(const AvlTree root);
Position FindMin(const AvlTree root);
AvlTree Insert(const ElementType e, AvlTree root);
AvlTree Delete(const ElementType e, AvlTree root);

void PrintInOrder(const AvlTree root);
void PrintWithHeightInOrder(const AvlTree root);
bool check_balance(const AvlTree root);
/*---------------------  end ----------------------*/

/* avl tree node define */
struct AvlNode {
    ElementType data;
    int         height;
    AvlTree     left;
    AvlTree     right;
};

