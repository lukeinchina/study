#ifndef DL_CNN_ACTIVATOR_H_
#define DL_CNN_ACTIVATOR_H_

struct ReluActivator {
    static double forward(double x) {
        return x > 0.0 ? x : 0
    }
    static double backward(double x) {
        return x>0 ? 1.0 : 0.0;
    }
};

#endif
