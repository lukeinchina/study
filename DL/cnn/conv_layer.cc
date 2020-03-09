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

static void 
print_filter(const Matrix3D flt) {
    printf("[\n");
    for (size_t i = 0; i < flt.dim3; i++) {
        printf("[\n");
        for (size_t j = 0; j < flt.dim2; j++) {
            printf("[");
            for (size_t k = 0; k < flt.dim1; k++) {
                printf("%f", flt.array[i][j][k]);
                printf("%c", k+1 == flt.dim1 ? ' ' : '\t');
            }
            printf("]\n");
        }
        printf("]\n");
    }
    printf("]\n");
    printf("--------\n");
}

/*----------------------------conv layer ----------------------------------*/
void cl_print_output(const ConvLayer *cl) {
    assert(cl->nflt == cl->feature_map.dim3);

    /* 输出卷积 */
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

    /* 输出旋转180°后的filter */
    for (size_t n = 0; n < cl->nflt; n++) {
        printf("\n------------- filpped filter【%lu】--------------\n", n);
        print_filter(cl->filpped_filters[n]);
    }
}

/*
 * @brief : 对输入矩阵进行周围补0扩展，生成新的矩阵。
 *          只对1、2维做0扩展，第3维不受影响。
 * @param zp [in] : zero padding, 表示行和列分别扩展的数目。
 * @return 扩展后的矩阵.
 *
 */
static Matrix3D 
padding_matrix(Matrix3D input, size_t zp) {
    Matrix3D dst;

    /* 第3维 保持不变 */
    dst.dim1  = input.dim1 + 2 * zp;
    dst.dim2  = input.dim2 + 2 * zp;
    dst.dim3  = input.dim3;
    dst.array = create_3d_array(dst.dim3, dst.dim2, dst.dim1);
    /* copy 原始输入内容到扩展矩阵中 */
    for (size_t i = 0; i < input.dim3; i++) {
        for (size_t j = 0; j < input.dim2; j++) {
            memcpy(dst.array[i][j+zp]+zp,
                   input.array[i][j], sizeof(double) * input.dim1);
        }
    }
    return dst;
}

/*
 *@brief: 对矩阵做180°旋转。第3维不受影响.
 */
static Matrix3D
create_rotated_matrix(Matrix3D m) {
    Matrix3D dst = m;
    dst.array = create_3d_array(m.dim3, m.dim2, m.dim1);

    for (size_t i = 0; i < m.dim3; i++) {
        double **from = m.array[i];
        double **to   = dst.array[i];

        for (size_t j = 0; j < m.dim2; j++) {
            for (size_t k = 0; k < m.dim3; k++) {
                to[j][k] = from[m.dim2-j-1][m.dim3-k-1];
            }
        }
    }
    return dst;
}

static Matrix3D *
create_filpped_filters(const Matrix3D *flts, size_t n) {
    Matrix3D *dst = (Matrix3D *)malloc(sizeof(Matrix3D) * n);
    for (size_t i = 0; i < n; i++) {
        dst[i] = create_rotated_matrix(flts[i]);
    }
    return dst;
}

void 
cl_forward(
          ConvLayer      *cl,     /* [in][out] */
          Matrix3D        input,  /* [in] */
          const Matrix3D *filters, /* [in] */
          size_t nflt
          ) {

    /* 记录属性，做误差计算时候用到 */
    cl->nflt         = nflt;
    cl->nchnl        = input.dim3;/* 第3维是通道数目 */
    cl->input_height = input.dim2;
    cl->input_width  = input.dim1;
    cl->flt_height   = (*filters).dim2;
    cl->flt_width    = (*filters).dim1;

    /* 扩展输入矩阵  */
    cl->padded_in = padding_matrix(input, cl->zero_padding);

    /* 计算输出矩阵大小 */
    size_t width  = calc_output_size(input.dim1, filters[0].dim1, 
                                     cl->zero_padding, cl->stride);
    size_t height = calc_output_size(input.dim2, filters[0].dim2,
                                     cl->zero_padding, cl->stride);
    cl->feature_map.dim3 = nflt;  /* filter 数目 */
    cl->feature_map.dim2 = height;
    cl->feature_map.dim1 = width;
    cl->feature_map.array = create_3d_array(nflt, height, width);

    /* 正式开始计算卷积 */
    for (size_t i = 0; i < nflt; i++) {
        Matrix2D out = {height, width, cl->feature_map.array[i]};
        cl_conv(cl->padded_in, filters[i], &out, cl->stride, cl->filter_bias);
    }
    /*--------------卷积计算到此结束----------------*/

    /* 生成旋转后的filter，后面计算误差时需要用到 */
    cl->filpped_filters = create_filpped_filters(filters, nflt);
    return ;
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

/*
 * @brief: 将步长为S的sensitivity map『还原』为步长为1的sensitivity map
 *
 */
static Matrix3D
cl_expand_sensitivity_map(ConvLayer *cl, Matrix3D sm) {
    size_t width  = cl->input_width - cl->flt_width + 1 - 2*cl->zero_padding;
    size_t height = cl->input_height - cl->flt_height + 1 - 2*cl->zero_padding;

    Matrix3D dst = {sm.dim3, height, width, NULL};
    dst.array = create_3d_array(dst.dim3, dst.dim2, dst.dim1);
    for (size_t i = 0; i < sm.dim3; i++) {
        for (size_t j = 0; j < sm.dim2; j++) {
            for (size_t k = 0; k < sm.dim1; k++) {
                size_t rpos = j * cl->stride;
                size_t cpos = k * cl->stride;
                dst.array[i][rpos][cpos] = sm.array[i][j][k];
            }
        }
    }
    return dst;
}

/*
 * @param sm [in]: sm == sensitivity map.
 *
 */
void
cl_bp_sensitivity_map(ConvLayer *cl, Matrix3D sm) {

    Matrix3D tmp = cl_expand_sensitivity_map(cl, sm);
    size_t width = tmp.dim1;
    /* 做完0扩展后的大小，使其和filter做卷积，输出大小等于原始输入矩阵大小 */
    /* zp * 2 + width + 1 - flt_width == input_width,该等式的变换 */
    size_t zp = (cl->input_width + cl->flt_width - 1 - width) / 2;
    cl->padded_sm = padding_matrix(tmp, zp);
    /* free 空间 */
    tmp.array = destroy_3d_array(tmp.array, tmp.dim3, tmp.dim2, tmp.dim1);

    /* dm = delta matrix. 同数据矩阵大小一样 */
    Matrix3D dm = {cl->nchnl, cl->input_height, cl->input_width, NULL};
    dm.array = create_3d_array(dm.dim3, dm.dim2, dm.dim1);

}
