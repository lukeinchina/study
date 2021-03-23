#include <stdlib.h>
#include <math.h>

#include <vector>

#include "types.h"
#include "util.h"
#include "lstm_layer.h"

/* ------------------------------------------------------------------------ */
static void lstm_delta_append(LstmLayer *lstm);

MyVector 
create_vector(size_t size) {
    MyVector v = {size, NULL};
    v.array = (double *)calloc(size, sizeof(double));
    return v;
}

int
add_vector(const MyVector &v1, const MyVector &v2,
        MyVector &v) {
    assert(v1.size == v2.size);
    assert(v1.size == v.size);

    if (v1.size != v2.size || v1.size != v.size) {
        return -1;
    }
    for (size_t i = 0; i < v.size; i++) {
        v.array[i] = v1.array[i] + v2.array[i];
    }
    return 0;
}

void *
destroy_vector(MyVector *v) { 
    free(v->array);
    v->size = 0;
    return NULL;
}

static int 
col_vec_add(const ColVector &v1, const ColVector &v2, ColVector *dst) {
    assert(v1.size == v2.size);
    if (v1.size != v2.size) {
        return -1;
    }
    for (size_t i = 0; i < v2.size; i++) {
        dst->array[i] = v1.array[i] + v2.array[i];
    }
    return 0;
}

static int
col_dot_row_vector(const ColVector &col, const RowVector &row,
        Matrix2D &mat) {
    for (size_t i = 0; i < col.size; i++) {
        for (size_t j = 0; j < row.size; j++) {
            mat.array[i][j] = col.array[i] * row.array[j];
        }
    }
    return 0;
}

/*---------------------------------------------------------*/

static struct Matrix2D 
create_matrix_2d(size_t row, size_t col) {
    Matrix2D m = {row, col, NULL};
    m.array = create_2d_array(row, col);
    return m;
}

static void 
destroy_matrix_2d(Matrix2D m) {
    m.array = destroy_2d_array(m.array, m.dim2, m.dim1);
}

/*---------------------------------------------------------*/
static struct WeightMat *
create_weight_mat(size_t input_width, size_t state_width) {
    struct WeightMat *w = new WeightMat;
    w->wh = create_matrix_2d(state_width, state_width);
    w->wx = create_matrix_2d(state_width, input_width);
    /* 设置随机权重 */
    set_random_value(w->wx);
    set_random_value(w->wh);

    w->b  = create_vector(state_width); /* 初试值为0 */
    memset(w->b.array, 0, w->b.size * sizeof(w->b.array[0]));

    return w;
}

static void *
destroy_weight_mat(struct WeightMat *wm) {
    destroy_matrix_2d(wm->wh);
    destroy_matrix_2d(wm->wx);
    destroy_vector(&(wm->b));
    delete wm;
    return NULL;
}
/*---------------------------------------------------------*/

/*
 * @brief: 创建状态向量。每个时刻一个状态，一个状态是N * 1的向量。
 *
 **/
static std::vector<ColVector> *
init_state_vec(size_t width) {
    ColVector v = create_vector(width);
    std::vector<ColVector> *vec = new std::vector<ColVector>;
    vec->push_back(v);
    return  vec;
}

LstmLayer *
lstm_create(size_t input_width, size_t state_width, 
                  double learning_rate) {
    struct LstmLayer *lstm = new LstmLayer;

    /* init */
    lstm->times = 0;
    lstm->input_width   = input_width;
    lstm->state_width   = state_width;
    lstm->learning_rate = learning_rate;

    /* init 各个时刻的单元状态向量  */
    lstm->c_vec  = init_state_vec(state_width);
    lstm->h_vec  = init_state_vec(state_width);
    lstm->f_vec  = init_state_vec(state_width);
    lstm->i_vec  = init_state_vec(state_width);
    lstm->o_vec  = init_state_vec(state_width);
    lstm->ct_vec = init_state_vec(state_width);

    /* init 遗忘门、输入门、输出门、单元状态 4个权重矩阵*/
    lstm->WF = create_weight_mat(input_width, state_width);
    lstm->WI = create_weight_mat(input_width, state_width);
    lstm->WO = create_weight_mat(input_width, state_width);
    lstm->WCT = create_weight_mat(input_width, state_width);
    /* 权重梯度 */
    lstm->WF_D = create_weight_mat(input_width, state_width);
    lstm->WI_D = create_weight_mat(input_width, state_width);
    lstm->WO_D = create_weight_mat(input_width, state_width);
    lstm->WCT_D = create_weight_mat(input_width, state_width);
    lstm->grad_w = create_weight_mat(input_width, state_width);

    /* lstm->delta_cvec  = init_state_vec(state_width); */
    lstm->delta_hvec  = new std::vector<ColVector>;
    lstm->delta_fvec  = new std::vector<ColVector>;
    lstm->delta_ivec  = new std::vector<ColVector>;
    lstm->delta_ovec  = new std::vector<ColVector>;
    lstm->delta_ctvec = new std::vector<ColVector>;
    /* 初始化一个时刻0的误差项. 否则后项计算访问到-1 */
    lstm_delta_append(lstm);
    /* 中间计算结果存储 */
    lstm->common = create_vector(lstm->state_width);
    lstm->temp   = create_vector(lstm->state_width);

    return lstm;
}

void *
lstm_destroy(LstmLayer **pp) {
    LstmLayer *lstm = *pp;
    ColVector item = {0, NULL};
    std::vector<ColVector> *list[] = {
        lstm->f_vec, lstm->i_vec, lstm->o_vec, 
        lstm->ct_vec, lstm->h_vec, lstm->c_vec, 
        lstm->delta_fvec, lstm->delta_ivec, lstm->delta_ovec,
        lstm->delta_ctvec, lstm->delta_hvec,
        NULL
    };
    for (int i = 0; NULL != list[i]; i++) {
        std::vector<ColVector> *ptr = list[i];
        while (ptr->size() > 0) {
            item = ptr->back();
            ptr->pop_back();
            destroy_vector(&item);
        }
        delete ptr;
    }

    WeightMat *wlist[] = {lstm->WF, lstm->WI, lstm->WO, lstm->WCT,
            lstm->WF_D, lstm->WI_D, lstm->WO_D, lstm->WCT_D, NULL};
    for (int i = 0; NULL != wlist[i]; i++) {
        wlist[i] = (WeightMat *)destroy_weight_mat(wlist[i]);
    }

    delete lstm;
    *pp = NULL;
    return NULL;
}

/*
 * @brief: 矩阵乘以列向量. 
 *         结果向量 存在dst中
 */
static int
matrix_dot_col_vec(const Matrix2D &m, const ColVector &v, ColVector *dst) {
    assert(NULL != dst);
    assert(m.dim1 == v.size);
    assert(m.dim2 == dst->size);

    size_t i, j;
    for (i = 0; i < m.dim2; i++) {
        double sum = 0.0; 
        const double *array = m.array[i];
        for (j = 0; j < m.dim1; j++) {
            sum += *(array +j) * dst->array[j];
        }
        dst->array[i] = sum;
    }
    return 0;
}

/*
 * @brief: [W(h), W(x)](T) * [H(t-1), X(t)]
 *         (T):矩阵转秩。
 *
 */
static int
lstm_calc_gate(LstmLayer *lstm, const WeightMat *w, const ColVector *x,
        int func_flag, ColVector *gate) {
    int err  = 0;
    int last = lstm->times - 1;
    const ColVector &h = lstm->h_vec->at(last); // 上次的LSTM输出
    /* W(h) * H(t-1). 维度是state_width */
    ColVector h_dst = create_vector(w->wh.dim2);
    matrix_dot_col_vec(w->wh, h, &h_dst);
    /* W(x) * X(t)    维度是input_width */
    ColVector x_dst = create_vector(w->wx.dim2);
    matrix_dot_col_vec(w->wx, *x, &x_dst);

    /* 向量相加的结果暂存在 h_dst */
    col_vec_add(h_dst, x_dst, &h_dst);
    col_vec_add(h_dst, w->b, &h_dst);

    /* sigmod(x) or tanh(x) */
    if (1 == func_flag) {
        for (size_t i = 0; i < h_dst.size; i++) {
            gate->array[i] = 1.0 / (1.0 + exp(-h_dst.array[i]));
        }
    }else if (2 == func_flag) {
        for (size_t i = 0; i < h_dst.size; i++) {
            gate->array[i] = 1.0 / (1.0 + exp(-h_dst.array[i]));
        }
    } else {
        err = -2;
    }
    destroy_vector(&x_dst);
    destroy_vector(&h_dst);
    return err;
}

int 
lstm_forward(LstmLayer *lstm, const ColVector *v) {
    size_t i = 0;
    lstm->times += 1;

    ColVector fgate  = create_vector(lstm->state_width);
    ColVector igate  = create_vector(lstm->state_width);
    ColVector ogate  = create_vector(lstm->state_width);
    ColVector ctgate = create_vector(lstm->state_width);
    ColVector c      = create_vector(lstm->state_width); /* 单元状态 */
    ColVector h      = create_vector(lstm->state_width); /* 输出 */
    lstm_calc_gate(lstm, lstm->WF, v, 1, &fgate);
    lstm_calc_gate(lstm, lstm->WI, v, 1, &igate);
    lstm_calc_gate(lstm, lstm->WO, v, 1, &ogate);
    lstm_calc_gate(lstm, lstm->WCT, v, 2, &ctgate);
    lstm->f_vec->push_back(fgate);
    lstm->i_vec->push_back(igate);
    lstm->o_vec->push_back(ogate);
    lstm->ct_vec->push_back(ctgate);

    /* 对应元素相乘  c(t) = f * c(t-1) + i * ct */
    const ColVector &clast = lstm->c_vec->at(lstm->times-1);
    assert(c.size == clast.size);
    for (i = 0; i < c.size; i++) {
        c.array[i] = fgate.array[i] * clast.array[i] + 
                     igate.array[i] * ctgate.array[i];
    }
    lstm->c_vec->push_back(c);

    /* tanh(c) * ogate */
    for (i = 0; i < ogate.size; i++) {
        double tanh = 2.0 / (1.0 + exp(-2 * c.array[i])) - 1.0;
        h.array[i] = ogate.array[i] * tanh;
    }
    lstm->h_vec->push_back(h);

    return 0;
}

static void 
row_dot_matrix_and_sum(const RowVector &row, const Matrix2D &mat,
        RowVector &dst) {
    assert(dst.size == mat.dim1);
    assert(row.size == mat.dim2);

    for (size_t i = 0; i < mat.dim2; i++) {
        double x = 0.0;
        for (size_t j = 0; j < mat.dim1; j++) {
            x += row.array[j] * mat.array[j][i];
        }
        dst.array[i] += x;
    }
    return;
}

/* -----------------------------------------------------------------*/
static void 
lstm_delta_append(LstmLayer *lstm) {
    std::vector<ColVector> *list[] = { lstm->delta_hvec,
         lstm->delta_fvec, lstm->delta_ivec, lstm->delta_ovec,
         lstm->delta_ctvec, NULL};
    for (size_t i = 0; NULL != list[i]; i++) {
        ColVector v = create_vector(lstm->state_width);
        list[i]->push_back(v);
    }
}

/* 
 * 根据k时刻的delta_h，计算k时刻的delta_f、
 * delta_i、delta_o、delta_ct，以及k-1时刻的delta_h.
 * k == 0时， k-1可能是负的. 必须保证k >= 1 
 */
static void 
lstm_calc_delta_k(LstmLayer *lstm, size_t k) {
    size_t i = 0;
    const ColVector &fgate = lstm->f_vec->at(k);
    const ColVector &igate = lstm->f_vec->at(k);
    const ColVector &ogate = lstm->f_vec->at(k);
    const ColVector &cgate = lstm->f_vec->at(k);
    const ColVector &ctgate = lstm->f_vec->at(k);
    const ColVector &cprev = lstm->f_vec->at(k-1);

    const ColVector &delta = lstm->delta_hvec->at(k);

    ColVector tanh_c = create_vector(lstm->state_width);
    for (i = 0; i < tanh_c.size; i++) {
        // tanh(x)
        tanh_c.array[i] = 2.0 / (1.0 + exp(-2 * cgate.array[i])) - 1.0;
    }

    /* 根据式  计算delta_o
     *  提取出来公共计算部分: common = δ * o(t) * (1 - tanh( c(t) )^2
     * 1) δ(o) = δ * tanh( o(t) ) * (1 - o(t));
     * 2) δ(f) = common * c(t-1) * f(t) * (1 - f(t))
     * 3) δ(i) = common * ct(t) * i(t) * (1 - i(t)) 
     * 4) δ(ct) = common * i(t) * (1 - ct(t)^2)
     * 
     * total: δ(t-1) = δ(o) * W(oh) + δ(i) * W(ih) + δ(f) * W(fh) + δ(ct) * W(ch)
     */
    /* common. 只是为了提升计算效率, 重复部分单独提出来计算 */
    for (i = 0; i < delta.size; i++) {
        double x = 1 - tanh_c.array[i];
        lstm->common.array[i] = delta.array[i] * ogate.array[i] * x * x;
    }
    ColVector &delta_o = lstm->delta_ovec->at(k);
    ColVector &delta_f = lstm->delta_fvec->at(k);
    ColVector &delta_i = lstm->delta_ivec->at(k);
    ColVector &delta_ct = lstm->delta_ctvec->at(k);
    for (i = 0; i < delta.size; i++) {
        double x = lstm->common.array[i];
        double y = ogate.array[i];
        delta_o.array[i]  = delta.array[i] * tanh_c.array[i] * y * (1 - y);
        delta_f.array[i]  = x * fgate.array[i] * (1 - fgate.array[i]) * cprev.array[i];   
        delta_i.array[i]  = x * igate.array[i] * (1 - igate.array[i]) * ctgate.array[i];
        delta_ct.array[i] = x * (1 - ctgate.array[i] * ctgate.array[i]) * igate.array[i];
    }

    /* 计算 δ(t-1) */
    ColVector &delta_prev = lstm->delta_hvec->at(k-1);
    memset(delta_prev.array, 0, sizeof(delta_prev.array[0]) * delta_prev.size);

    /* trick. 
     * (1xN) * (NxN) = (1xN), 结果是一行N列的向量，需要再转秩成Nx1的列向量 
     * 但由于我的实现都是vector只是名字不同，所以就没有再转秩，
     * 只是换了名字，CovVector <-----> RowVector
     * 由于4个向量做累加，为提升效率，就都直接把dot结果累加到delta_prev上。
     */
    row_dot_matrix_and_sum(delta_o, lstm->WO->wh, delta_prev);
    row_dot_matrix_and_sum(delta_f, lstm->WF->wh, delta_prev);
    row_dot_matrix_and_sum(delta_i, lstm->WI->wh, delta_prev);
    row_dot_matrix_and_sum(delta_ct, lstm->WCT->wh, delta_prev);
}

static void 
lstm_calc_delta(LstmLayer *lstm, const ColVector *delta_h) {
    assert(delta_h->size == lstm->state_width);

    /* 初始化各个时刻的误差项 */
    lstm_delta_append(lstm);
    // size_t index = lstm->times - 1;
    /* 可能会forward次数比backward多，times 不一定等于delta_vec.size() */
    int index = lstm->delta_hvec->size() - 1;
    /* 保存从上一层传递下来的当前时刻的误差项 */
    memcpy(lstm->delta_hvec->at(index).array, delta_h->array, 
            sizeof(delta_h->array[0]) * delta_h->size);
    /* 迭代计算每个时刻的误差项 */
    for (; index > 1; index--) {
        lstm_calc_delta_k(lstm, index);
    }
}

static void 
lstm_calc_gradient_k(LstmLayer *lstm, int k) {
    /* transpose */
    RowVector h = lstm->delta_hvec->at(k-1);

    /* 计算各个门的梯度  */
    col_dot_row_vector(lstm->delta_fvec->at(k), h, lstm->grad_w->wh);
    /* 累加各个时刻的梯度。 */
    matrix_add(lstm->grad_w->wh, lstm->WF_D->wh, lstm->WF_D->wh);
    /* 偏置项的梯度就是 delta . 累加各个时刻的梯度 */
    add_vector(lstm->delta_fvec->at(k), lstm->WF_D->b, lstm->WF_D->b);

    col_dot_row_vector(lstm->delta_ivec->at(k), h, lstm->grad_w->wh);
    matrix_add(lstm->grad_w->wh, lstm->WI_D->wh, lstm->WI_D->wh); /* 累加 */
    add_vector(lstm->delta_ivec->at(k), lstm->WI_D->b, lstm->WI_D->b);

    col_dot_row_vector(lstm->delta_ovec->at(k), h, lstm->grad_w->wh);
    matrix_add(lstm->grad_w->wh, lstm->WO_D->wh, lstm->WO_D->wh); /* 累加 */
    add_vector(lstm->delta_ovec->at(k), lstm->WO_D->b, lstm->WO_D->b);

    col_dot_row_vector(lstm->delta_ctvec->at(k), h, lstm->grad_w->wh);
    matrix_add(lstm->grad_w->wh, lstm->WCT_D->wh, lstm->WCT_D->wh); /* 累加 */
    add_vector(lstm->delta_ctvec->at(k), lstm->WCT_D->b, lstm->WCT_D->b);
}

void lstm_calc_gradient(LstmLayer *lstm, const ColVector *x) {
    /* 第0次，即初试状态不会计算 */ 
    for (int k = lstm->times; k > 0; k--) {
        lstm_calc_gradient_k(lstm, k);
    }
    /* 计算对本次输入x的权重梯度 */
    int curr = lstm->times - 1;
    RowVector xt = *x;
    col_dot_row_vector(lstm->delta_fvec->at(curr), xt, lstm->WF_D->wx);
    col_dot_row_vector(lstm->delta_ivec->at(curr), xt, lstm->WI_D->wx);
    col_dot_row_vector(lstm->delta_ovec->at(curr), xt, lstm->WO_D->wx);
    col_dot_row_vector(lstm->delta_ctvec->at(curr), xt, lstm->WCT_D->wx);
}

void 
lstm_backward(LstmLayer *lstm, const ColVector *x, const ColVector *delta_h) {
    lstm_calc_delta(lstm, delta_h);
    lstm_calc_gradient(lstm, x);
    return;
}

void lstm_update(LstmLayer *lstm) {
    double rate = lstm->learning_rate;
    size_t i, j;
    /* 状态 */
    for (i = 0; i < lstm->state_width; i++) {
        for (j = 0; j < lstm->state_width; j++) {
            lstm->WF->wh.array[i][j] -= rate * lstm->WF->wh.array[i][j];
            lstm->WI->wh.array[i][j] -= rate * lstm->WI->wh.array[i][j];
            lstm->WO->wh.array[i][j] -= rate * lstm->WO->wh.array[i][j];
            lstm->WCT->wh.array[i][j] -= rate * lstm->WCT->wh.array[i][j];
        }
    }
    /* 输入矩阵 */
    for (i = 0; i < lstm->state_width; i++) {
        for (j = 0; j < lstm->input_width; j++) {
            lstm->WF->wx.array[i][j] -= rate * lstm->WF->wx.array[i][j];
            lstm->WI->wx.array[i][j] -= rate * lstm->WI->wx.array[i][j];
            lstm->WO->wx.array[i][j] -= rate * lstm->WO->wx.array[i][j];
            lstm->WCT->wx.array[i][j] -= rate * lstm->WCT->wx.array[i][j];
        }
    }
    /* bias */
    for (i = 0; i < lstm->state_width; i++) {
        lstm->WF->b.array[i] -= rate * lstm->WF->b.array[i];
        lstm->WI->b.array[i] -= rate * lstm->WI->b.array[i];
        lstm->WO->b.array[i] -= rate * lstm->WO->b.array[i];
        lstm->WCT->b.array[i] -= rate * lstm->WCT->b.array[i];
    }
    return ;
}

/* for debug */
void lstm_reset_state(LstmLayer **pp) {
    LstmLayer *p = *pp;
    LstmLayer *lstm = lstm_create(p->input_width, p->state_width,
                   p->learning_rate);
    lstm_destroy(pp);
    *pp = lstm;
}
