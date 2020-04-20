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
 *
 * @note  [in] 注释变量是需要输入的参数；
 *        [inner]注释变量是为了减少内存申请、拷贝等操作,提高效率而内部用到变量，
 *        [out]注释变量是最后需要输出的结果变量.
 *
 */
struct ConvLayer{

    /* 所有size_t类型变量(即注释为[in])为初始化CL需要输入的参数. */
    size_t zero_padding;        /* [in] */
    size_t stride;              /* [in] */
    double rate;                /* [in] 学习速率 */

    /* filter相关 */
    size_t nflt;                /* [in] */
    size_t flt_height;          /* [in] */
    size_t flt_width;           /* [in] */

    /* 输入相关 */
    size_t nchnl;               /* [in] */
    size_t input_height;        /* [in] */
    size_t input_width;         /* [in] */

    /* 和filter相关的变量, 中间计算用到。 */
    double   *flt_bias;         /* [out] */
    double   *bias_gradient;    /* [out] 每个filter的bias 的梯度*/
    Matrix3D *flt_gradient;     /* [inner] filter权重的梯度*/
    Matrix3D *filpped_filters;  /* [inner] 旋转180°后的filter */
    Matrix3D *filters;          /* */

    /* 输入数据相关的变量 */
    Matrix3D derivative_input;  /* [inner] 输入矩阵值的偏导 */
    Matrix3D padded_input;      /* [inner] 周边添加0值扩展后的输入 */

    Matrix3D stride1_sm;        /* [inner] 1 步长还原后的sensitivity map */
    Matrix3D padded_sm;         /* [inner] stride1_sm 经过 zero padding 之后的matrix */

    Matrix3D tmp_delta;         /* [inner] 临时变量，保存1个filter带来的误差 */

    Matrix3D input_delta;       /* [out] 输入matrix的delta. 反向传播计算生成 */
    Matrix3D feature_map;       /* [out] 即卷积层的输出 */
};

/* 
 *@brief: 前向计算。不需要传递filters，使用卷积中自带的filters
 *        默认使用该接口。
 */
void 
cl_forward(
          ConvLayer       *cl,     /* [in][out] */
          const Matrix3D  &input   /* [in] */
          );

/*
 *@brief: 前向计算卷积。
 *        使用传递过来的filters参数，该接口主要用于自测。
 *        一般情况应该是不带filters参数的forward接口.
 */
void 
cl_forward(
          ConvLayer      *cl,      /* [in][out] */
          Matrix3D        input,   /* [in] */
          const Matrix3D *filters, /* [in] */
          size_t          nflt
          );

void cl_backward(ConvLayer *cl, const Matrix3D sm);

/* 
 *@brief: 后向传播计算中包含了前向计算，调用该接口的话
 *        不需要再单独调动forward();
 */
void cl_backward( ConvLayer *cl,
                  Matrix3D        input,   /* [in] */
                  const Matrix3D *filters, /* [in] */
                  size_t          nflt,
                  const Matrix3D  sm
                );


int cl_create(ConvLayer *lc);
void cl_destroy(ConvLayer *cl);

void cl_print_output(const ConvLayer *cl);
void cl_bp_gradient(ConvLayer *cl, Matrix3D sm);

Matrix3D *create_filters(size_t num, size_t depth, size_t height, size_t width);
Matrix3D *destroy_filters(Matrix3D *fitlers, size_t num);

#endif
