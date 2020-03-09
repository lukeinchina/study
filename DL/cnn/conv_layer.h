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
    double filter_bias;         /* [in] */
    double rate;                /* [in] 学习速率 */

    size_t nchnl;               /* channel 数目. 方便查看 */
    size_t nflt;                /* filter 数目 . 方便查看 */
    size_t flt_height;
    size_t flt_width;
    size_t input_height;
    size_t input_width;


    Matrix3D *filpped_filters;  /* 旋转180°后的filter */
    Matrix3D padded_in;         /* 周边添加0值扩展后的输入 */
    Matrix3D feature_map;       /* 即卷积层的输出 */
    Matrix3D sensitivity_map;   /* 即输出  的误差 */
    Matrix3D padded_sm;         /* zero padding 之后的sensitivety map */
};

void 
cl_conv(
        Matrix3D        input,   /* [in]  */
        Matrix3D        flt,     /* [in]  */
        Matrix2D       *output,  /* [out] */
        size_t          stride, 
        double          flt_bias 
        );
void 
cl_forward(
          ConvLayer      *cl,      /* [in][out] */
          Matrix3D        input,   /* [in] */
          const Matrix3D *filters, /* [in] */
          size_t          nflt
          );

void cl_destroy(ConvLayer *cl);
void cl_print_output(const ConvLayer *cl);

Matrix3D *create_filters(size_t num, size_t depth, size_t height, size_t width);
Matrix3D *destroy_filters(Matrix3D *fitlers, size_t num);

#endif
