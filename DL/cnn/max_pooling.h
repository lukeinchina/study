#ifndef DL_CNN_MAX_POOLING_H_
#define DL_CNN_MAX_POOLING_H_

#include "util.h"

struct MaxPooling {
    size_t nchnl;
    size_t input_width;
    size_t input_height;
    size_t flt_width;
    size_t flt_height;
    size_t stride;

    /* --------分界线-------- */
    size_t output_width;
    size_t output_height;

    Matrix3D feature_map; /* output */
    Matrix3D input_delta; /* 误差值 */
};

int  mp_create(MaxPooling *mp);
void mp_destroy(MaxPooling *mp);
void mp_forward(MaxPooling *mp, const Matrix3D &input);
void mp_backward(MaxPooling *mp, const Matrix3D &input, const Matrix3D &sm);

#endif
