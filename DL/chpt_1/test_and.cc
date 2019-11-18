#include <stdio.h>

#include "perceptron.h"

double and_func(double x)
{
    return  (x > 0.0) ? 1.0 : 0.0;
}

int
main(int argc, char *argv[])
{
    Perceptron tron;
    perceptron_init(&tron, 2, and_func);

    vector<Sample> samples;

    Sample s1, s2, s3, s4;
    s1.push_back(1);
    s1.push_back(1);
    samples.push_back(s1);

    s2.push_back(1);
    s2.push_back(0);
    samples.push_back(s2);

    s3.push_back(0);
    s3.push_back(1);
    samples.push_back(s3);

    s4.push_back(0);
    s4.push_back(0);
    samples.push_back(s4);

    int ls[] = {1, 0, 0, 0};
    vector<double> labels;
    for (int i = 0; i < 4; i++) {
        labels.push_back(ls[i] * 1.0);
    }

    perceptron_train(&tron, 10, 0.1, samples, labels);

    printf("1 and 1 = %f\n",  perceptron_predict(&tron, s1));
    printf("0 and 0 = %f\n",  perceptron_predict(&tron, s4));
    printf("1 and 0 = %f\n",  perceptron_predict(&tron, s2));
    printf("0 and 1 = %f\n",  perceptron_predict(&tron, s3));

    return 0;
}
