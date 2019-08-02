#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

struct ListNode {
    int vertex;
    struct ListNode *next;
};

struct AdjTable {
    struct ListNode *table;
    int size;
};

void add_edge(AdjTable *adj, int start, int end) {
    struct ListNode *ptr = (struct ListNode *)malloc(sizeof(struct ListNode));
    assert(NULL != ptr);
    ptr->vertex = end;

    ptr->next = (adj->table)[start].next;
    (adj->table)[start].next = ptr;
    printf("[%d]--->[%d]\n", start, end);
}

bool canFinish(int numCourses, int** prerequisites, int prerequisitesRowSize, int *prerequisitesColSizes) {
    int row   = prerequisitesRowSize;
    int *cols = prerequisitesColSizes;
    int i,j;

    /* init adj table */
    struct AdjTable adj;
    adj.table = (struct ListNode *)calloc(numCourses, sizeof(struct ListNode));
    adj.size  = numCourses;
    for (i = 0; i < numCourses; i++) {
        adj.table[i].vertex = i;
    }

    for (i = 0; i < row; i++) {
        for (j = 1; j < cols[i]; j++) {
            add_edge(&adj, prerequisites[i][j], prerequisites[i][0]);
        }
    }


    struct ListNode *ptr, *q;
    int *indegrees = (int *)calloc(numCourses, sizeof(int));
    /* find these vertex which's indegree == 0 */
    for (i = 0; i < adj.size; i++) {
        ptr = adj.table[i].next;
        for (; NULL != ptr; ptr = ptr->next) {
            indegrees[ptr->vertex] += 1;
        }
    }
    int flag = 1;
    while (flag) {
        printf("---------------------------\n");
        flag = 0;
        for (i = 0; i < numCourses; i++) {
            printf("{%d} indegree[%d]\n", i, indegrees[i]);
            /* be dependent */
            if (indegrees[i] != 0) {
                continue;
            }
            for (ptr = adj.table[i].next; NULL != ptr;) {
                printf("[%d]-->[%d]\n", i, ptr->vertex);
                flag = 1;
                q    = ptr;
                ptr  = ptr->next;
                indegrees[q->vertex] -= 1;
                free(q);
            }
        }
    }

    /* finally. */
    bool succ = true;
    for (i = 0; i < numCourses; i++) {
        for (ptr = adj.table[i].next; NULL != ptr;) {
            q    = ptr;
            ptr  = ptr->next;
            succ = false;
            free(q);
        }
    }
    free(indegrees);
    free(adj.table);

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
    pp[1][1] = 2;
    pp[2][0] = 0;
    pp[2][1] = 2;

    printf("[%d]\n", canFinish(3, pp, ROW, cols));

    return 0;
}
