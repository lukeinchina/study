#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "util.h"
#include "conv_layer.h"

/*--------------------------------filter -----------------------------------*/
Matrix3D *
create_filters(size_t num, size_t depth, size_t height, size_t width) {
    Matrix3D *filters = (Matrix3D *)malloc(sizeof(Matrix3D) * num);
    for (size_t i = 0; i < num; i++) {
        filters[i].array = create_3d_array(depth, height, width);
        filters[i].dim3  = depth;
        filters[i].dim2  = height;
        filters[i].dim1  = width;
    }
    return filters;
}

Matrix3D *
destroy_filters(Matrix3D *filters, size_t num) {
    for (size_t i = 0; i < num; i++) {
        destroy_3d_array(filters[i].array, filters[i].dim3, 
                filters[i].dim2, filters[i].dim1);
    }
    free(filters);
    return NULL;
}

/*----------------------------conv layer ----------------------------------*/
void cl_print_output(const ConvLayer *cl) {
    assert(cl->nflt == cl->feature_map.dim3);

    for (size_t n = 0; n < cl->nflt; n++) {
        printf("[%lu]:\n", n);
        printf("[\n");
        for (size_t i = 0; i < cl->feature_map.dim2; i++) {
            printf("[");
            for (size_t j = 0; j < cl->feature_map.dim1; j++) {
                printf("%f", cl->feature_map.array[n][i][j]);
                printf("%c", j+1 == cl->feature_map.dim1 ? ' ' : '\t');
            }
            printf("]\n");
        }
        printf("]\n");
        printf("--------\n");
    }
}

void 
cl_forward(
          ConvLayer      *cl,     /* [in][out] */
          Matrix3D        input,  /* [in] */
          const Matrix3D *filters, /* [in] */
          size_t nflt
          ) {

    size_t i,j;
    size_t nchnl  = input.dim3;  /* 第3维是通道数目 */
    cl->nflt      = nflt;
    cl->nchnl     = nchnl;

    /* 生成扩展输入矩阵  */
    size_t width  = input.dim1 + 2 * cl->zero_padding;
    size_t height = input.dim2 + 2 * cl->zero_padding;
    cl->padded_in.dim1  = width;
    cl->padded_in.dim2  = height;
    cl->padded_in.dim3  = nchnl;
    cl->padded_in.array = create_3d_array(nchnl, height, width);
    /* copy 原始输入内容到扩展矩阵中 */
    for (i = 0; i < input.dim3; i++) {
        for (j = 0; j < input.dim2; j++) {
            memcpy(cl->padded_in.array[i][j+cl->zero_padding]+cl->zero_padding,
                   input.array[i][j], sizeof(double) * input.dim1);
        }
    }

    /* 计算输出矩阵大小 */
    width  = calc_output_size(input.dim1, filters[0].dim1, 
                                     cl->zero_padding, cl->stride);
    height = calc_output_size(input.dim2, filters[0].dim2,
                                     cl->zero_padding, cl->stride);
    cl->feature_map.dim3 = nflt;  /* filter 数目 */
    cl->feature_map.dim2 = height;
    cl->feature_map.dim1 = width;
    cl->feature_map.array = create_3d_array(nflt, height, width);

    /* 正式开始计算卷积 */
    for (i = 0; i < nflt; i++) {
        Matrix2D out = {height, width, cl->feature_map.array[i]};
        cl_conv(cl->padded_in, filters[i], &out, cl->stride, cl->filter_bias);
    }
}

static double 
cl_conv_elem(Matrix3D input, Matrix3D flt, size_t roff, size_t coff) {
    double sum = 0.0;
    for (size_t i = 0; i < flt.dim3; i++) {
    /* 在每个channel上计算，然后累加各channel计算的和 */

        for (size_t j = 0; j < flt.dim2; j++) {
            for (size_t k = 0; k < flt.dim1; k++) {
                sum += input.array[i][roff+j][coff+k] * flt.array[i][j][k];
            }
        }
    }
    return sum;
}

/*
 * @brief: 卷积计算，cross correlation. 不是数学上的卷积计算。
 *
 * @note: stride, bias两个参数必须单独出来，作为参数传递到卷积的计算函数中。
 *        是因为在前向卷积计算时候要用卷积计算，同时在后向传播计算误差时
 *        也需要用到卷积的计算，但后向计算时和前向计算时这两个参数不同。
 *        后向计算误差时，stride=1，bias=0，是固定模式。
 */
void cl_conv(
            Matrix3D        input,   /* [in]  */
            Matrix3D        flt,     /* [in]  */
            Matrix2D       *output,  /* [out] */
            size_t          stride, 
            double          flt_bias 
            ) {
    for (size_t i = 0; i < output->dim2; i++) {
        for (size_t j = 0; j < output->dim1; j++) {
            double  sum = cl_conv_elem(input, flt, i*stride, j*stride);
            /* 应用激活函数计算 */
            // output->array[i][j] =  relu_activator(sum + flt_bias);
            output->array[i][j] =  sum + flt_bias;
        }
    }
}


void 
cl_destroy(ConvLayer *cl) {
    (void )cl;
    return ;
}
/*
 * 计算，相当于把第层的sensitive map周围补一圈0，在与180度翻转后的filter进行cross-correlation
 */
