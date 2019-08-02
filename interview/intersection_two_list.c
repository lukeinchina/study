#include <stdio.h>
#include <stdlib.h>

/**
 *  * Definition for singly-linked list.
 *       */
struct ListNode {
    int val;
    struct ListNode *next;
};

struct ListNode *getIntersectionNode(struct ListNode *headA, struct ListNode *headB) {
    struct ListNode *endA, *endB;
    int lenA = 1;
    int lenB = 1;
    if (NULL == headA || NULL == headB) {
        return NULL;
    }

    for (endA = headA; NULL != endA->next; endA = endA->next) {
        lenA++;
    }
    for (endB = headB; NULL != endB->next; endB = endB->next) {
        lenB++;
    }
    if (endA != endB) {
        return NULL;
    }
    while (lenA < lenB) {
        headB = headB->next;
        lenB--;
    }
    while (lenB < lenA) {
        headA = headA->next;
        lenA--;
    }
    while (headA != headB) {
        headA = headA->next;
        headB = headB->next;
    }

    return headA;
}

struct ListNode *
InsertList(struct ListNode *head, int v) {
    struct ListNode *ptr = (struct ListNode *)malloc(sizeof(struct ListNode));
    ptr->val = v;
    ptr->next = head;
    return ptr;
}

void PrintList(struct ListNode *head) {
    while (NULL != head) {
        printf("%d\t", head->val);
        head = head->next;
    }
    printf("\n");
}

int
main(void) {
    int array1[] = {9,7,5,3,1};
    int array2[] = {10,8,6,4,2};
    struct ListNode *head1 = NULL;
    struct ListNode *head2 = NULL;
    struct ListNode *ptr = NULL;
    int i  = 0;
    for (i = 0; i < 5; i++) {
        head1 = InsertList(head1, array1[i]);
    }
    // head2 = head1;
    for (i = 0; i < 5; i++) {
        head2 = InsertList(head2, array2[i]);
    }
    PrintList(head1);
    PrintList(head2);
    ptr = getIntersectionNode(head2, head1);
    printf("intersection node [%d]\n", ptr == NULL ? -1 : ptr->val);
    return 0;
}
