#include "lstm_layer.h"

double error_sum(const ColVector &vec) {
    double sum = 0.0;
    for (size_t i = 0; i < vec.size; i++) {
        sum += vec.array[i];
    }
    return sum;
}

void gradient_check(double rate) {
    LstmLayer *lstm = lstm_create(3, 2, rate);
    double input1[] = {1.0, 2.0, 3.0};
    double input2[] = {2.0, 3.0, 4.0};
    double d[] = {1.0, 1.0};

    ColVector case1 = {3, input1};
    ColVector case2 = {3, input2};
    ColVector delta = {2, d};

    lstm_forward(lstm, &case1);
    lstm_forward(lstm, &case2);
    lstm_backward(lstm, &case2, &delta);

    /* */
    Matrix2D actual_delta = {3, 3, NULL};
    actual_delta.array = create_2d_array(3, 3); 
    Matrix2D from = lstm->WF_D->wh;
    for (size_t i = 0; i < 3; i++) {
        memcpy(actual_delta.array[i], from.array[i], sizeof(double) * 3);
    }

    const double epsilon = 0.0001;
    Matrix2D mat = lstm->WF->wh;
    for (size_t i = 0; i < mat.dim2; i++) {
        for (size_t j = 0; j < mat.dim1; j++) {
            mat.array[i][j] += epsilon;
            lstm_reset_state(&lstm);
            lstm_forward(lstm, &case1);
            lstm_forward(lstm, &case2);
            double err1 = error_sum(lstm->h_vec->at(1));
            mat.array[i][j] -= 2 * epsilon;
            lstm_reset_state(&lstm);
            lstm_forward(lstm, &case1);
            lstm_forward(lstm, &case2);
            double err2 = error_sum(lstm->h_vec->at(1));
            double expect_grad = (err1 - err2) / (2 * epsilon);
            printf("[%ld][%ld] expected - actural:%f - %f\n", i, j, 
                    expect_grad, from.array[i][j]);
            mat.array[i][j] += epsilon;
        }
    }
    return ;
}

int
main(int argc, char *argv[]) {
    double rate = 0.001;
    if (argc > 1) {
        rate = atof(argv[1]);
    }
    gradient_check(rate);
    return 0;
}
