/*
 * =====================================================================================
 *
 *       Filename:  tire_test.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/14/2014 03:55:05 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  luke (), lukeinchina@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "tire.h"

int
main(void) {
    char buff[1024];
    int  top = 0;
    int  i   = 0;
    TireTree root = NULL;
    root = InsertTire(root, "hello");
    root = InsertTire(root, "here");
    root = InsertTire(root, "her");
    root = InsertTire(root, "hello,world");
    root = InsertTire(root, "world");
    void PrintAllWord(TireTree);
    PrintAllWord(root);
    root = DestoryTire(root);
    return 0;
}
