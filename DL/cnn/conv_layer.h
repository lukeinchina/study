/*
 *
 *
 * 申明：文本实现参考自文章：
 * https://www.zybuluo.com/hanbingtao/note/485480
 *
 */
#ifndef DL_CNN_CONV_LAYER_H_
#define DL_CNN_CONV_LAYER_H_

#include "types.h"
#include "config.h"

/* 
 * @brief: 只考虑3维情况。二维是3维的一个特例
 *         即channel 多于1个，filter多于1个。
 */
struct ConvLayer{

    size_t zero_padding;        /* [in] */
    size_t stride;              /* [in] */
    double rate;                /* [in] 学习速率 */

    size_t nchnl;               /* channel 数目. 方便查看 */
    size_t nflt;                /* filter 数目 . 方便查看 */
    size_t flt_height;
    size_t flt_width;
    size_t input_height;
    size_t input_width;


    double   *flt_bias;         /* */
    double   *bias_gradient;    /* 每个filter的bias 的梯度*/
    Matrix3D *flt_gradient;     /* filter权重的梯度*/
    Matrix3D *filpped_filters;  /* 旋转180°后的filter */

    Matrix3D derivative_input;  /* 输入矩阵值的偏导 */
    Matrix3D padded_in;         /* 周边添加0值扩展后的输入 */

    Matrix3D feature_map;       /* 即卷积层的输出 */
    Matrix3D sensitivity_map;   /* 即输出  的误差 */
    Matrix3D padded_sm;         /* zero padding 之后的sensitivety map */
    Matrix3D delta;             /* 输入matrix的delta. 反向传播计算生成 */
};

void 
cl_forward(
          ConvLayer      *cl,      /* [in][out] */
          Matrix3D        input,   /* [in] */
          const Matrix3D *filters, /* [in] */
          size_t          nflt
          );

void cl_backward(ConvLayer *cl,
          Matrix3D        input,   /* [in] */
          const Matrix3D *filters, /* [in] */
          size_t          nflt,
          const Matrix3D  sm);


void cl_destroy(ConvLayer *cl);
void cl_print_output(const ConvLayer *cl);
void cl_bp_gradient(ConvLayer *cl, Matrix3D sm);

Matrix3D *create_filters(size_t num, size_t depth, size_t height, size_t width);
Matrix3D *destroy_filters(Matrix3D *fitlers, size_t num);

#endif
