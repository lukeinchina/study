#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "types.h"
#include "util.h"

size_t 
calc_output_size(size_t ninput, size_t nflt,
        size_t npadding, size_t nstride) {
    return (ninput - nflt + 
            2 * npadding) / nstride + 1 ;
}

void 
set_random_value(double **array, size_t row, size_t col) {
    /* have sramdom() before this */
    for (size_t i = 0; i < row; i++) {
        for (size_t j = 0; j < col; j++) {
            /* [-0.001, +0.001] */
            array[i][j] = (random() % 2000 - 1000) / 1000000.0;
        }
    }
}

double **
create_2d_array(size_t row, size_t col) {
    double **array = (double **)malloc(sizeof(double *) * row);
    for (size_t i = 0; i < row; i++) {
        array[i] = (double *)calloc(col, sizeof(double));
    }
    return array;
}

double **
destroy_2d_array(double **array, size_t row, size_t col) {
    (void)col;
    for (size_t i = 0; i < row; i++) {
        free(array[i]);
    }
    free(array);
    return NULL;
}

void
print_2d_array(double **array, size_t row, size_t col) {
    printf("[\n");
    for (size_t i = 0; i < row; i++) {
        printf("\t[");
        for (size_t j = 0; j < col; j++) {
            printf("%f%c", array[i][j], j+1 == col ? ']' : '\t');
        }
        printf("\n");
    }
    printf("]\n");
}
/*-----------------------------2d -------------------------------*/

/*-----------------------------3D -------------------------------*/
double ***
create_3d_array(size_t dim3, size_t dim2, size_t dim1) {
    double ***array = (double ***) malloc(sizeof(void *) * dim3);
    for (size_t i = 0; i < dim3; i++) {
        array[i] = create_2d_array(dim2, dim1);
    }
    return array;
}

double ***
destroy_3d_array(double ***array, size_t dim3, size_t dim2, size_t dim1) {
    for (size_t i = 0; i < dim3; i++) {
        array[i] = destroy_2d_array(array[i], dim2, dim1);
    }
    free(array);
    return NULL;
}

void 
set_random_value(Matrix2D m) {
    set_random_value(m.array, m.dim2, m.dim1);
    return ;
}

void 
set_random_value(Matrix3D m) {
    for (size_t i = 0; i < m.dim3; i++) {
        set_random_value(m.array[i], m.dim2, m.dim1);
    }
    return ;
}

/*
 * @param m1 [in]
 * @param m2 [in]
 * @param m  [out]
 *
 * @note: m可能是m1或者m2，也可能是一个新的matrix
 */
void
matrix_add(Matrix3D m1, Matrix3D m2, Matrix3D m) {
    assert(m1.dim3 == m2.dim3);
    assert(m1.dim2 == m2.dim2);
    assert(m1.dim1 == m2.dim1);
    for (size_t i = 0; i < m.dim3; i++) {
        for (size_t j = 0; j < m.dim2; j++) {
            for (size_t k = 0; k < m.dim1; k++) {
                m.array[i][j][k] = m1.array[i][j][k] + m2.array[i][j][k];
            }
        }
    }
}
