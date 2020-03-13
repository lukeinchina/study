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

void check_forward(ConvLayer *cl, const Matrix3D input, Matrix3D *filters, size_t flt_num);
void check_gradient(ConvLayer *cl, const Matrix3D input, Matrix3D *filters, size_t flt_num, const Matrix3D sm);

Matrix3D *create_filter(void);
Matrix3D  create_input(void);
Matrix3D  create_sensitivity_map(void);

int
main(void) {
    srandom(time(NULL));

    ConvLayer cl;
    memset(&cl, 0, sizeof(cl));
    cl.zero_padding = ZERO_PADDING;
    cl.stride       = STRIDE;
    // cl.flt_bias  = BIAS;
    cl.rate         = LEARNING_RATE;

    Matrix3D *filters = create_filter();
    Matrix3D  input   = create_input();
    Matrix3D  sm      = create_sensitivity_map();

    check_forward(&cl, input, filters, FILTER_NUM);

    filters = destroy_filters(filters, FILTER_NUM);
    cl_destroy(&cl);

    return 0;
}

void check_forward(ConvLayer *cl, const Matrix3D input, 
        Matrix3D *filters, size_t flt_num) {
    cl_forward(cl, input, filters, FILTER_NUM);
    cl_print_output(cl);
}

void check_gradient(ConvLayer *cl, const Matrix3D input, 
        Matrix3D *filters, size_t flt_num, const Matrix3D sm) {

    cl_forward(cl, input, filters, FILTER_NUM);
    cl_bp_sensitivity_map(cl, sm);
}

Matrix3D *create_filter(void) {

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

Matrix3D  create_sensitivity_map(void) {
    Matrix3D output = {FILTER_NUM, 3, 3, NULL};
    output.array = create_3d_array(FILTER_NUM, 3, 3);
    for (size_t i = 0; i < FILTER_NUM; i++) {
        for (size_t j = 0; j < 3; j++) {
            for (size_t k = 0; k < 3; k++) {
                output.array[i][j][k] = 1.0;
            }
        }
    }
    return output; 
}

