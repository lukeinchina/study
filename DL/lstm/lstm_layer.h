#ifndef LSTM_LSTM_LAYER_H_
#define LSTM_LSTM_LAYER_H_

#include <vector>

#include "types.h"
#include "util.h"

struct MyVector {
    size_t size;
    double *array;
};
typedef struct MyVector ColVector;
typedef struct MyVector RowVector;

struct WeightMat {
    Matrix2D wh;
    Matrix2D wx;
    ColVector b;  /* 本质是一 列向量 */
};

struct LstmLayer {
    size_t input_width;
    size_t state_width;
    double learning_rate;
    size_t times;

    std::vector<ColVector> *f_vec;  /* 各个时刻的遗忘门f       */
    std::vector<ColVector> *i_vec;  /* 各个时刻的输入门i       */
    std::vector<ColVector> *c_vec;  /* 各个时刻的单元状态向量c */
    std::vector<ColVector> *h_vec;  /* 各个时刻的输出向量h     */
    std::vector<ColVector> *o_vec;  /* 各个时刻的输出门o       */
    std::vector<ColVector> *ct_vec; /* 各个时刻的即时状态c~    */

    WeightMat *WF; /*遗忘门权重矩阵Wfh, Wfx, 偏置项bf    */
    WeightMat *WI; /* 输入门权重矩阵Wih, Wix, 偏置项bf   */
    WeightMat *WO; /* 输出门权重矩阵Woh, Wox, 偏置项bf   */
    WeightMat *WCT; /* 单元状态权重矩阵Wch, Wcx, 偏置项bf */
    WeightMat *WF_D;
    WeightMat *WI_D;
    WeightMat *WO_D;
    WeightMat *WCT_D;
    WeightMat *grad_w; /* 中间变量 */

    /* backward */
    std::vector<ColVector> *delta_hvec;
    std::vector<ColVector> *delta_fvec;
    std::vector<ColVector> *delta_ivec;
    std::vector<ColVector> *delta_ovec;
    std::vector<ColVector> *delta_ctvec;
    ColVector common;
    RowVector temp;
};

/*---------------------------------- function -------------------------------*/
void lstm_reset_state(LstmLayer **pp);
LstmLayer *lstm_create(size_t input_width, size_t state_width, double rate=0.001);
void *lstm_destroy(LstmLayer **pp);
int  lstm_forward(LstmLayer *lstm, const ColVector *v);
void lstm_backward(LstmLayer *lstm, const ColVector *x, const ColVector *delta);
void lstm_update(LstmLayer *lstm);


MyVector create_vector(size_t size);
/*---------------------------------- function end----------------------------*/
#endif
