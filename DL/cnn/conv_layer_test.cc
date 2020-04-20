#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "conv_layer.h"
#include "util.h"

#define CHANNEL_NUM 3
#define FILTER_NUM 2

#define FMATRIX_SIZE  3
#define INMATRIX_SIZE 5


/* ----------------------------------------------------------------*/
double filter_w1[CHANNEL_NUM][FMATRIX_SIZE][FMATRIX_SIZE] = {
    {
        {-1, 1, 0},
        {0,  1, 0},
        {0,  1, 1}
    },
    {
        {-1, -1, 0},
        {0,  0,  0},
        {0,  -1, 0}
    },
    {
        {0,  0, -1},
        {0,  1,  0},
        {1, -1, -1}
    }
}; /*, bias = -1; */

double filter_w2[CHANNEL_NUM][FMATRIX_SIZE][FMATRIX_SIZE] = {
    {
        {1,  1, -1},
        {-1, -1, 1},
        {0,  -1, 1}
    },
    {
        {0,  1,  0},
        {-1, 0, -1},
        {-1, 1,  0}
    },
    {
        {-1, 0, 0},
        {-1, 0, 1},
        {-1, 0, 0}
    }
}; /*, bias = 0; */


double input[CHANNEL_NUM][INMATRIX_SIZE][INMATRIX_SIZE] = {
    {
        {0, 1, 1, 0, 2},
        {2, 2, 2, 2, 1},
        {1, 0, 0, 2, 0},
        {0, 1, 1, 0, 0},
        {1, 2, 0, 0, 2}
    },
    {
        {1, 0, 2, 2, 0},
        {0, 0, 0, 2, 0},
        {1, 2, 1, 2, 1},
        {1, 0, 0, 0, 0},
        {1, 2, 1, 1, 1}
    },
    {
        {2, 1, 2, 0, 0},
        {1, 0, 0, 1, 0},
        {0, 2, 1, 0, 1},
        {0, 1, 2, 2, 2},
        {2, 1, 0, 0, 1}
    }
}; 

/* ----------------------------------------------------------------*/

void check_forward(ConvLayer *cl, const Matrix3D input, const Matrix3D *filters, size_t flt_num);
void check_gradient(ConvLayer *cl, const Matrix3D input,
                Matrix3D *filters, const Matrix3D sm);

Matrix3D *create_filters(void);
Matrix3D  create_input(void);

int
main(void) {
    srandom(time(NULL));

    ConvLayer cl;
    memset(&cl, 0, sizeof(cl));
    cl.zero_padding = ZERO_PADDING;
    cl.stride       = STRIDE;
    cl.rate         = LEARNING_RATE;
    cl.nchnl        = CHANNEL_NUM;
    cl.input_width  = INMATRIX_SIZE;
    cl.input_height = INMATRIX_SIZE;
    cl.nflt         = FILTER_NUM;
    cl.flt_width    = FMATRIX_SIZE;
    cl.flt_height   = FMATRIX_SIZE;

    Matrix3D *filters = create_filters();
    Matrix3D  input   = create_input();

    cl_create(&cl);
    // check_forward(&cl, input, filters, FILTER_NUM);
    // exit(0);

    Matrix3D sm = {cl.nflt, cl.feature_map.dim2, cl.feature_map.dim1, NULL};
    sm.array    = create_3d_array(sm.dim3, sm.dim2, sm.dim1);
    matrix_set(sm, 1.0);

    cl_forward(&cl, input, filters, FILTER_NUM);
    cl_backward(&cl, sm);

    check_gradient(&cl, input, filters, sm);

    filters = destroy_filters(filters, FILTER_NUM);
    cl_destroy(&cl);

    return 0;
}

void check_forward(ConvLayer *cl, const Matrix3D input, 
        const Matrix3D *filters, size_t flt_num) {
    cl_forward(cl, input, filters, flt_num);
    cl_print_output(cl);
}

void check_gradient(ConvLayer *cl, const Matrix3D input,
        Matrix3D *filters, const Matrix3D sm) {
    double epsilon = 0.0001;
    Matrix3D flt = filters[0];
    Matrix3D grad = cl->flt_gradient[0];

    cl_backward(cl, sm);

    for (size_t i = 0; i < grad.dim3; i++) {
        for (size_t j = 0; j < grad.dim2; j++) {
            for (size_t k = 0; k < grad.dim1; k++) {
                flt.array[i][j][k] += epsilon;
                cl_forward(cl, input, filters, FILTER_NUM);
                double err1 = matrix_elem_sum(cl->feature_map);
                flt.array[i][j][k] -= 2 * epsilon;
                cl_forward(cl, input, filters, FILTER_NUM);
                double err2 = matrix_elem_sum(cl->feature_map);

                double expect_grad = (err1 - err2) / (2 * epsilon);
                printf("weight(%lu:%lu:%lu) expect_grad:%f, actual grad:%f\n",
                        i, j, k, expect_grad, grad.array[i][j][k]);
                /* */
                flt.array[i][j][k] += epsilon;
            }
        }
    }
}

Matrix3D *create_filters(void) {

    Matrix3D *filters = create_filters(FILTER_NUM, CHANNEL_NUM, FMATRIX_SIZE, FMATRIX_SIZE);

    size_t i,j;
    /* 把filter 1的权重改写了测试case 1 */
    for (i = 0; i < CHANNEL_NUM; i++) {
        for (j = 0; j < FMATRIX_SIZE; j++) {
            double *from = filter_w1[i][j];
            double *to   = filters[0].array[i][j];
            memcpy(to, from, sizeof(double) * FMATRIX_SIZE);
        }
    }
    /* 把filter 2的权重改写了测试case 2 */
    for (i = 0; i < CHANNEL_NUM; i++) {
        for (j = 0; j < FMATRIX_SIZE; j++) {
            double *from = filter_w2[i][j];
            double *to   = filters[1].array[i][j];
            memcpy(to, from, sizeof(double) * FMATRIX_SIZE);
        }
    }

    return filters;
}

Matrix3D  create_input(void) {
    Matrix3D in = {CHANNEL_NUM, INMATRIX_SIZE, INMATRIX_SIZE, NULL};
    in.array = create_3d_array(CHANNEL_NUM, INMATRIX_SIZE, INMATRIX_SIZE);
    for (size_t i = 0; i < CHANNEL_NUM; i++) {
        for (size_t j = 0; j < INMATRIX_SIZE; j++) {
            memcpy(in.array[i][j], input[i][j], sizeof(double) * INMATRIX_SIZE);
        }
    }
    return in;
}

