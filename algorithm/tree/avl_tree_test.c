/*
 * =====================================================================================
 *
 *       Filename:  avl_tree_test.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/06/2014 04:02:17 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  luke (), lukeinchina@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include <stdio.h>
#include "avl_tree.h"


int
main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;

    AvlTree root = NULL;
    int i = 0;
    int elems[10] = {5, 4, 3, 2, 1, 15, 8, 9, 12, 0};
    for (i = 0; i < 10; i++) {
        root = Insert(elems[i], root);
        check_balance(root);
    }
    PrintInOrder(root);
    printf("\n");
    PrintWithHeightInOrder(root);
    printf("\n");

    int del_elems[6] = {3, 2, 1, 9, 12, 15};
    for (i = 0; i < 6; i++) {
        root= Delete(del_elems[i], root);
        check_balance(root);
        printf("\n");
        PrintWithHeightInOrder(root);
    }
    printf("\n");
    return 0;
}
