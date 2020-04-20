#include <stdlib.h>

#include <vector>

#include "types.h"
#include "config.h"
#include "util.h"
#include "conv_layer.h"
#include "cnn.h"

CNN *cnn_create() {
    CNN *cnn = (CNN *)calloc(1, sizeof(CNN));
    /* macro defination in config.h */

    cnn->cl       = (ConvLayer *)calloc(CONV_NUM, sizeof(ConvLayer));
    cnn->pooling  = (MaxPooling *)calloc(CONV_NUM, sizeof(MaxPooling));
    for (int i = 0; i < CONV_NUM; i++) {
        /* 设置卷积层参数 */
        cl_create(cnn->cl + i);
        /* 设置pooling 层参数 */
        mp_create(cnn->pooling + i);
    }

    std::vector<int> layer_counts;
    layer_counts.push_back(FC_INPUT_NUM);
    layer_counts.push_back(FC_HIDDEN_NUM);
    layer_counts.push_back(FC_OUTPUT_NUM);
    cnn->fc = new Network(layer_counts);
    
    return cnn;
}

void *
cnn_destroy(CNN *cnn) {
    for (int i = 0; i < CONV_NUM; i++) {
        cl_destroy(cnn->cl + i);
        mp_destroy(cnn->pooling + i);
    }
    free(cnn->cl);
    free(cnn->pooling);
    delete cnn->fc;
    free(cnn);
    return NULL;
}

void 
cnn_train(CNN *cnn, const Matrix3D &input, const Label &label) {
    int i = 0;
    /* 预测  */
    for (i = 0; i < CONV_NUM; i++) {
        cl_forward(cnn->cl + i, input);
        // mp_forward(cnn->pooling + i, input);
    }

    /* 把卷积层输出转换成神经网络层的输入 */
    Sample fc_input;
    cnn->fc->train_one_sample(fc_input, label);

    /* 反向计算误差，更新权重 */
    for (i = CONV_NUM - 1; i >= 0; i--) {
        // mp_backward(cnn->pooling + i, input);
        // cl_backward(cnn->cl + i, input);
    }
}

void 
cnn_predict(CNN *cnn, const Matrix3D &input, Label &label) {
    for (int i = 0; i < CONV_NUM; i++) {
        cl_forward(cnn->cl + i, input);
        // mp_forward(cnn->pooling + i, input);
    }
    /* 把卷积层输出转换成神经网络层的输入 */
    Sample fc_input;
    cnn->fc->predict(fc_input);
    cnn->fc->get_output(label);
    return ;
}
