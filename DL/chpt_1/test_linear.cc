#include <stdio.h>

#include "perceptron.h"

double linear_func(double x)
{
    return  x;
}

int
main(int argc, char *argv[])
{
    Perceptron tron;
    perceptron_init(&tron, 2, linear_func);

    vector<Sample> samples;

    Sample s1, s2, s3, s4;
    s1.push_back(1);
    s1.push_back(1);
    samples.push_back(s1);

    s2.push_back(2);
    s2.push_back(3);
    samples.push_back(s2);

    s3.push_back(4);
    s3.push_back(5);
    samples.push_back(s3);

    s4.push_back(6);
    s4.push_back(8);
    samples.push_back(s4);

    /* 2 * x1 + x2  */
    int ls[] = {3, 7, 15, 20};
    vector<double> labels;
    for (int i = 0; i < 4; i++) {
        labels.push_back(ls[i] * 1.0);
    }

    perceptron_train(&tron, 10, 0.01, samples, labels);

    printf("1 op 1 = %f\n",  perceptron_predict(&tron, s1));
    printf("2 op 3 = %f\n",  perceptron_predict(&tron, s2));
    printf("4 op 5 = %f\n",  perceptron_predict(&tron, s3));
    printf("6 op 8 = %f\n",  perceptron_predict(&tron, s4));

    return 0;
}
