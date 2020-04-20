#include <stdio.h>

#include "max_pooling.h"

#define CHNL_NUM 2

#define IN_HEIGHT 4
#define IN_WIDTH 4

#define FLT_HEIGHT  2
#define FLT_WIDTH 2
#define STRIDE 2

Matrix3D input;
Matrix3D sm;

MaxPooling init_max_pooling(void);


int
main(void) {
    MaxPooling mp = init_max_pooling();
    mp_forward(&mp, input);
    printf("-----feature map:-------\n");
    print_matrix(mp.feature_map);

    mp_backward(&mp, input, sm);
    printf("\n------input delta------\n");
    print_matrix(mp.input_delta);
    mp_destroy(&mp);
    return 0;
}

MaxPooling
init_max_pooling(void) {
    MaxPooling mp;
    mp.nchnl = CHNL_NUM;
    mp.flt_width = FLT_WIDTH;
    mp.flt_height = FLT_HEIGHT;
    mp.input_height = IN_HEIGHT;
    mp.input_width  = IN_WIDTH;
    mp.stride  = STRIDE;
    mp_create(&mp);

    int array[CHNL_NUM][IN_HEIGHT][IN_WIDTH] = {
        {
          {1,1,2,4},
          {5,6,7,8},
          {3,2,1,0},
          {1,2,3,4}
        },
        {
          {0,1,2,3},
          {4,5,6,7},
          {8,9,0,1},
          {3,4,5,6}
        }
    };

    int out_array[CHNL_NUM][FLT_HEIGHT][FLT_WIDTH] = {
        {
            {1,2},
            {2,4}
        },
        {
            {3,5},
            {8,2}
        }
    };

    int i, j, k;
    input.dim3 = CHNL_NUM;
    input.dim2 = IN_HEIGHT;
    input.dim1 = IN_WIDTH;
    input.array = create_3d_array(CHNL_NUM, IN_HEIGHT, IN_WIDTH);
    for (i = 0; i < CHNL_NUM; i++) {
        for (j = 0; j < IN_HEIGHT; j++) {
            for (k = 0; k < IN_WIDTH; k++) {
                input.array[i][j][k] = array[i][j][k];
            }
        }
    }

    sm.dim3 = CHNL_NUM;
    sm.dim2 = FLT_HEIGHT;
    sm.dim1 = FLT_WIDTH;
    sm.array = create_3d_array(CHNL_NUM, FLT_HEIGHT, FLT_WIDTH);
    for (i = 0; i < CHNL_NUM; i++) {
        for (j = 0; j < FLT_HEIGHT; j++) {
            for (k = 0; k < FLT_WIDTH; k++) {
                sm.array[i][j][k] = out_array[i][j][k];
            }
        }
    }

    return mp;
}

