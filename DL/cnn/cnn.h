#ifndef DL_CNN_CNN_H_
#define DL_CNN_CNN_H_

#include "types.h"
#include "util.h"
#include "conv_layer.h"
#include "max_pooling.h"
#include "network.h"

struct CNN {
    ConvLayer  *cl;
    MaxPooling *pooling;

    Network    *fc;
};

CNN *cnn_create();

void cnn_train(CNN *cnn, const Matrix3D &input, const Label &label);
void cnn_predict(CNN *cnn, const Matrix3D &input, Label &label);

void *cnn_destory(CNN *cnn);

#endif
