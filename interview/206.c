#include <stdio.h>
#include <stdlib.h>

/**
 * Definition for singly-linked list.
 */
struct ListNode {
	int val;
	struct ListNode *next;
};

struct ListNode* reverseList(struct ListNode* head) {
	struct ListNode *p = head;
	struct ListNode *q = p;
	for (head = NULL; NULL != p; p = q) {
		q = p->next;
		p->next = head;
		head = p;
	}
	return head;
}

/* insert element at head */
struct ListNode *insertElements(struct ListNode *head, int val) {
	struct ListNode *ptr = (struct ListNode *)malloc(sizeof(struct ListNode));
	ptr->val  = val;
	ptr->next = head;
	return ptr;
}

void print_list(struct ListNode *head) {
	while (NULL != head) {
		printf("%d ", head->val);
		head = head->next;
	}
	printf("\n");
}

int
main(void) {
	/* int array[] = {1,2,3,4,5,6}; */
	int array[] = {16};
	int i;
	struct ListNode *head = NULL;
	for (i = sizeof(array) / sizeof(array[0]) - 1; i >= 0; i--) {
		// head = insertElements(head, array[i]);
	}
	print_list(head);
	head = reverseList(head);
	print_list(head);
	return 0;
}
