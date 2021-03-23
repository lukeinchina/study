#ifndef DL_LSTM_ACTIVATOR_H_
#define DL_LSTM_ACTIVATOR_H_

#include <math.h>

struct SigmoidActivator {
    /* sigmod 函数 */
    double forward(double x) {
        return 1.0 / (1.0 + exp(-x)); 
    }
    /* sigmod 导数  */
    double backward(double x) {
        return x * (1 - x);
    }
};

struct TanhActivator {
    /* tanh(x) 函数  */
    double forward(double x) {
        return 2.0 / (1.0 + exp(-2 * x)) - 1.0;
    }
    /* tanh(x) 导数  */
    double backward(double x) {
        return 1 - x * x;
    }
};

#define
