#include <stdio.h>
#include <stdlib.h>

/**
 * Definition for singly-linked list.
 */
struct ListNode {
	int val;
	struct ListNode *next;
};

struct ListNode* removeElements(struct ListNode* head, int val) {
	struct ListNode temp  = {0, head};
	struct ListNode *prev = &temp;
	struct ListNode *ptr  = prev->next;
	while (NULL != ptr) {
		if (val == ptr->val) {
			prev->next = ptr->next;
			free(ptr);
			ptr = prev->next;
		} else {
			prev = ptr;
			ptr  = ptr->next;
		}
	}
	return temp.next;
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
	int array[] = {1,2,6,3,4,5,6};
	int i;
	struct ListNode *head = NULL;
	for (i = sizeof(array) / sizeof(array[0]) - 1; i >= 0; i--) {
		head = insertElements(head, array[i]);
	}
	print_list(head);
	head = removeElements(head, 1);
	head = removeElements(head, 2);
	head = removeElements(head, 6);
	print_list(head);
	return 0;
}
