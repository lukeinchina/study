#include <stdio.h>
#include <time.h>
#include "search_tree.h"


int
main(int argc, char *argv[]) 
{
    (void)argc;
    (void)argv;

    srand(time(NULL));
    int i = 0;
    int array[10] = {1, 3, 5, 7, 9, 10, 8, 6, 4, 2};
    SearchTree root = NULL;
    for (i = 0; i < 10; i++) {
        // root = Insert(rand(), root);
        root = Insert(array[i], root);
    }

    PrintInOrder(root);
    printf("\nMax:");
    Position p = FindMax(root);
    PrintElement(p->data);
    printf("\nMin:");
    p = FindMin(root);
    PrintElement(p->data);
    printf("\n");

    printf("delete:\n");
    root = Delete(5, root);
    root = Delete(10, root);
    root = Delete(1, root);
    PrintInOrder(root);
    printf("\n");

    printf("\nFinish\n");
    MakeEmpty(root);
    return 0;
}
