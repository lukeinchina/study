#ifndef _DL_PERCEPTRON_H_
#define _DL_PERCEPTRON_H_


#include <vector>

using namespace std;

typedef vector<double> Sample;
typedef double (*Activator)(double);

struct Perceptron {
    double         bias;
    vector<double> weights;
    Activator      activator; /* 激活函数 */
};

int perceptron_init(Perceptron *p, int n, Activator f);

int perceptron_train(Perceptron *p, int rounds, double rate,
        const vector<Sample> &samples, 
        const vector<double> &labels);

double perceptron_predict(Perceptron *p, const Sample &sample);

#endif
