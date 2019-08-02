#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

struct LinkedNode {
	int vertex;
	struct LinkedNode *next;
};

void add_edge(struct LinkedNode *table, int start, int end) {
	struct LinkedNode *ptr = (struct LinkedNode *)malloc(sizeof(struct LinkedNode));
	assert(NULL != ptr);
	ptr->vertex = end;

	ptr->next = table[start].next;
	table[start].next = ptr;
}

bool canFinish(int numCourses, int** prerequisites, int prerequisitesRowSize, int *prerequisitesColSizes) {
	int row   = prerequisitesRowSize;
	int *cols = prerequisitesColSizes;
	int i,j;

	/* init adj table */
	struct LinkedNode *table = (struct LinkedNode *)calloc(numCourses, sizeof(struct LinkedNode));
	for (i = 0; i < numCourses; i++) {
		table[i].vertex = i;
	}

	for (i = 0; i < row; i++) {
		for (j = 1; j < cols[i]; j++) {
			add_edge(table, prerequisites[i][j], prerequisites[i][0]);
		}
	}


	struct LinkedNode *ptr;
	int *indegrees = (int *)calloc(numCourses, sizeof(int));
	/* find these vertex which's indegree == 0 */
	for (i = 0; i < numCourses; i++) {
		ptr = table[i].next;
		for (; NULL != ptr; ptr = ptr->next) {
			indegrees[ptr->vertex] += 1;
		}
	}
	int flag = 1;
	while (flag) {
		flag = 0;
		for (i = 0; i < numCourses; i++) {
			/* be dependent */
			if (indegrees[i] != 0) {
				continue;
			}
			while (NULL != table[i].next) {
				flag = 1;
				ptr  = table[i].next;
				table[i].next = ptr->next;
				indegrees[ptr->vertex] -= 1;
				free(ptr);
			}
		}
	}

	/* finally. */
	bool succ = true;
	for (i = 0; i < numCourses; i++) {
		while (NULL != table[i].next) {
			ptr  = table[i].next;
			table[i].next = ptr->next;
			succ = false;
			free(ptr);
		}
	}
	free(indegrees);
	free(table);

	return succ;
}

int
main(void) {
	static const int ROW = 3;
	int **pp = (int **)calloc(ROW, sizeof(char *));
	int *cols = (int *)calloc(ROW, sizeof(int));
	for (int i = 0; i < ROW; i++) {
		cols[i] = 2;
		pp[i] = (int *)calloc(cols[i], sizeof(int));
	}
	pp[0][0] = 0;
	pp[0][1] = 1;
	pp[1][0] = 1;
	pp[1][1] = 0;
	pp[2][0] = 0;
	pp[2][1] = 2;

	printf("[%d]\n", canFinish(3, pp, ROW, cols));

	return 0;
}
