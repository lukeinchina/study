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

/*
 * @brief: 两个二维的矩阵进行卷积计算.
 * @param roff [in]: 输入矩阵的行offset
 * @param coff [in]: 输入矩阵的列offset
 * @return : 两个相同大小的矩阵中，每个元素相乘的结果后再加和.
 *
 */
static double 
cl_conv_elem(Matrix2D input, Matrix2D flt, size_t roff, size_t coff) {
    double sum = 0.0;

    for (size_t i = 0; i < flt.dim2; i++) {
        for (size_t j = 0; j < flt.dim1; j++) {
            sum += input.array[roff+i][coff+j] * flt.array[i][j];
        }
    }
    return sum;
}

static double 
cl_conv_elem(Matrix3D input, Matrix3D flt, size_t roff, size_t coff) {
    double sum = 0.0;
    for (size_t i = 0; i < flt.dim3; i++) {
        /* 在每个channel上计算，然后累加各channel计算的和 */
        Matrix2D in = {input.dim2, input.dim1, input.array[i]};
        Matrix2D f  = {flt.dim2, flt.dim1, flt.array[i]};
        sum += cl_conv_elem(in, f, roff, coff);
    }
    return sum;
}

/*
 *@brief: 2维上的卷积计算。
 */
static void 
cl_conv(
        Matrix2D        input,   /* [in]  */
        Matrix2D        flt,     /* [in]  */
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

/*
 * @brief: 卷积计算，cross correlation. 不是数学上的卷积计算。
 *
 * @note: stride, bias两个参数必须单独出来，作为参数传递到卷积的计算函数中。
 *        是因为在前向卷积计算时候要用卷积计算，同时在后向传播计算误差时
 *        也需要用到卷积的计算，但后向计算时和前向计算时这两个参数不同。
 *        后向计算误差时，stride=1，bias=0，是固定模式。
 */
static void 
cl_conv(
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
cl_forward(
          ConvLayer      *cl,      /* [in][out] */
          Matrix3D        input,   /* [in] */
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

    cl->flt_bias = (double *)calloc(nflt, sizeof(double));
    /* 正式开始计算卷积 */
    for (size_t i = 0; i < nflt; i++) {
        Matrix2D out = {height, width, cl->feature_map.array[i]};
        cl_conv(cl->padded_in, filters[i], &out, cl->stride, cl->flt_bias[i]);
    }
    /*--------------卷积计算到此结束----------------*/

    /* 下面这些都是误差时需要用到, 提前计算好 */

    /* 1. 生成旋转180°后的filter */
    cl->filpped_filters = create_filpped_filters(filters, nflt);
    /* 2. 对输入计算导数. */
    cl->derivative_input = padding_matrix(input, 0);
    matrix_elem_wise(cl->derivative_input, relu_derivator);

    return ;
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
 * @brief: 反向传播，计算误差，更新权重。
 *
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

    /* dm = delta matrix. 同输入矩阵大小一样. 中间变量 */
    Matrix3D dm = {cl->nchnl, cl->input_height, cl->input_width, NULL};
    dm.array    = create_3d_array(dm.dim3, dm.dim2, dm.dim1);

    cl->delta       = dm;
    cl->delta.array = create_3d_array(dm.dim3, dm.dim2, dm.dim1); 

    for (size_t i = 0; i < cl->nflt; i++) {
        Matrix2D in = {cl->padded_sm.dim2, cl->padded_sm.dim1, cl->padded_sm.array[i]};
        for (size_t j = 0; j < cl->nchnl; j++) {
            /* 2D 的filter */
            Matrix2D flt = {cl->flt_height, cl->flt_width, cl->filpped_filters[i].array[j]};
            Matrix2D out = {dm.dim2, dm.dim1, dm.array[j]};
            cl_conv(in, flt, &out, 1, 0);
        }
        matrix_add(cl->delta, dm, cl->delta);
    }
    dm.array = destroy_3d_array(dm.array, dm.dim3, dm.dim2, dm.dim1);

    /* delta *= input  将计算结果与激活函数的偏导数做 乘法操作 */
    for (size_t i = 0; i < cl->nchnl; i++) {
        for (size_t j = 0; j < cl->input_height; j++) {
            for (size_t k = 0; k < cl->input_width; k++) {
                cl->delta.array[i][j][k] *= cl->derivative_input.array[i][j][k];
            }
        }
    }
}

void 
cl_update_filter(ConvLayer *cl, Matrix3D *filters, size_t nflt) {
    assert(nflt == cl->nflt);

    size_t height = filters[0].dim2;
    size_t width  = filters[0].dim1;

    for (size_t f = 0; f < nflt; f++) {
        for (size_t c = 0; c < cl->nchnl; c++) {
            for (size_t i = 0; i < height; i++) {
                for (size_t j = 0; j < width; j++) {
                    filters[f].array[c][i][j] -= cl->rate * cl->flt_gradient[f].array[c][i][j];
                }
            }
        }
        cl->flt_bias[f] -= cl->rate * cl->bias_gradient[f];
    }
}

void 
cl_bp_gradient(ConvLayer *cl, Matrix3D sm) {
    /* 创建filter的梯度迭代用到的变量空间. 初始化为0 */
    cl->bias_gradient = (double *)calloc(cl->nflt, sizeof(double));
    cl->flt_gradient  = (Matrix3D *)malloc(cl->nflt * sizeof(Matrix3D));
    for (size_t i = 0; i < cl->nflt; i++) {
        Matrix3D *p = cl->flt_gradient + i;
        p->dim3     = cl->nchnl;
        p->dim2     = cl->flt_height;
        p->dim1     = cl->flt_width;
        p->array    = create_3d_array(p->dim3, p->dim2, p->dim1);
    }

    /* 步长为1的扩展矩阵 */
    Matrix3D ex_sm = cl_expand_sensitivity_map(cl, sm);
    for (size_t f = 0; f < cl->nflt; f++) {
        /* 在这个卷积计算中，扩展收入相当于filter. 
         * 而原来的filter在这里变成是output。
         */
        Matrix2D flt = {ex_sm.dim2, ex_sm.dim1, ex_sm.array[f]};

        for (size_t c = 0; c < cl->nchnl; c++) {
            Matrix2D in  = {cl->padded_in.dim2, cl->padded_in.dim1, cl->padded_in.array[c]};
            Matrix2D out = {cl->flt_height, cl->flt_width, cl->flt_gradient[f].array[c]};
            cl_conv(in, flt, &out, 1, 0);
        }
        cl->bias_gradient[f] = matrix_elem_sum(flt);
    }
}


void cl_backward(ConvLayer *cl,
          Matrix3D        input,   /* [in] */
          const Matrix3D *filters, /* [in] */
          size_t          nflt,
          const Matrix3D  sm) {
    cl_forward(cl, input, filters, nflt);
    cl_bp_sensitivity_map(cl, sm);
    cl_bp_gradient(cl, sm);
}
