/*
 * =====================================================================================
 *
 *       Filename:  ac_tire_test.c
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
#include "ac_tire.h"

int
main(void) {
    ACTireTree root = NULL;
    // say she shr he her
    root = InsertTire(root, "say");
    root = InsertTire(root, "she");
    root = InsertTire(root, "her");
    root = InsertTire(root, "he");
    root = InsertTire(root, "shr");
    root = InsertTire(root, "yes");
    void PrintAllWord(ACTireTree);
    PrintAllWord(root);
    BuildAcAutomation(root);
    root = DestoryTire(root);
    return 0;
}
