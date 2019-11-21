#include <stdio.h>
#include <assert.h>

#include "perceptron.h"

int perceptron_init(Perceptron *p, int n, Activator f)
{
    p->bias      = 1.0;
    p->activator = f;
    p->weights.resize(n, 1.0);
    return 0;
}

double
perceptron_predict(Perceptron *p, const Sample &sample)
{
    assert(p->weights.size() == sample.size());

    double val = 0.0;
    for (size_t i = 0; i < sample.size(); i++) {
        val += p->weights[i] * sample[i];
    }
    return p->activator(val + p->bias);
}

static void
update_weight(Perceptron *p, double rate, const Sample &sample, 
        double predict_val, double real_val)
{
    assert(sample.size() == p->weights.size());

    double delta = real_val - predict_val;
    p->bias += rate * delta;
    for (size_t i = 0; i < sample.size(); i++) {
        p->weights[i] += rate * delta * sample[i];
    }
}

void perceptron_print_args(const Perceptron *p)
{
    printf("weights:");
    for (size_t i = 0; i < p->weights.size(); i++) {
        printf("%.3f\t", p->weights[i]);
    }
    printf("\nbias: %.3f\n", p->bias);
}

static void
iterate_one(Perceptron *p, double rate,
        const Sample &sample, double label)
{
    double val = perceptron_predict(p, sample);
    update_weight(p, rate, sample, val, label);
}

static int 
iterate_samples(Perceptron *p, double rate,
        const vector<Sample> &samples,
        const vector<double> &labels)
{
    size_t size = samples.size();
    for (size_t i = 0; i < size; i++) {
        iterate_one(p, rate, samples[i], labels[i]);
    }
    return 0;
}

int
perceptron_train(Perceptron *p, int rounds, double rate,
        const vector<Sample> &samples, 
        const vector<double> &labels)
{
    if (samples.size() != labels.size()) {
        return -1;
    }

    for (int i = 0; i < rounds; i++) {
        iterate_samples(p, rate, samples, labels);
    };
    return 0;
}
