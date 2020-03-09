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

int
main(void) {
    srandom(time(NULL));

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

    double input_w1[CHANNEL_NUM][INMATRIX_SIZE][INMATRIX_SIZE] = {
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

    ConvLayer cl;
    memset(&cl, 0, sizeof(cl));
    cl.zero_padding = ZERO_PADDING;
    cl.stride       = STRIDE;
    cl.filter_bias  = BIAS;
    cl.rate         = LEARNING_RATE;

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


    Matrix3D in = {CHANNEL_NUM, INMATRIX_SIZE, INMATRIX_SIZE, NULL};
    in.array = create_3d_array(CHANNEL_NUM, INMATRIX_SIZE, INMATRIX_SIZE);
    for (i = 0; i < CHANNEL_NUM; i++) {
        for (j = 0; j < INMATRIX_SIZE; j++) {
            memcpy(in.array[i][j], input_w1[i][j], sizeof(double) * INMATRIX_SIZE);
        }
    }

    cl_forward(&cl, in, filters, FILTER_NUM);
    cl_print_output(&cl);

    filters = destroy_filters(filters, FILTER_NUM);
    cl_destroy(&cl);
    return 0;
}
