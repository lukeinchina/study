#include <stdio.h>
#include <assert.h>

#include <vector>

#include "node.h"
#include "network.h"

/* 标准差：0.5 * ∑(Ti = Yi)^2  */
double network_error(const std::vector<double> &vec1, 
        const std::vector<double> &vec2) {
    double error = 0;
    assert(vec1.size() == vec2.size());
    for (size_t i = 0; i < vec1.size(); i++) {
        double delta = vec1[i] - vec2[i];
        error += delta * delta;
    }
    return error * 0.5;
}

void 
gradient_check(Network *net, const std::vector<double> &sample, 
        const std::vector<double> &labels) {

    net->get_gradient(sample, labels);

    for (size_t i = 0; i < net->_conns.size(); i++) {
        double epsilon = 0.0001; 
        std::vector<double> output;

        /* 对每一个梯度进行check */
        Connection *conn = net->_conns[i];
        double gradient = conn->get_gradient();

        /* 增加一个很小的值，计算网络误差 */
        conn->_weight += epsilon;
        net->predict(sample);
        net->get_output(output);
        double err1 = network_error(output, labels);

        /* 减小一个很小的值，计算网路误差 */
        conn->_weight -= 2* epsilon;
        net->predict(sample);
        net->get_output(output);
        double err2 = network_error(output, labels);

        /* 函数在x处的导数值，约等于在x的领域内变化一个很小值的极限。*/
        double expected = (err2 - err1) / (2 * epsilon);
        printf("expect value:%f, actual value:%f\n", expected, gradient);
    }
}

int
main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("usage: %s n1 n2 n3 ...\n", argv[0]);
        return 1;
    }
    int node_cnt = 0;
    int i;
    std::vector<int> layers;
    for (i = 1; i < argc; i++) {
        node_cnt = atoi(argv[i]);
        assert(node_cnt > 0);
        layers.push_back(node_cnt);
    }

    Network *net = new Network(layers);

    std::vector<double> sample;
    std::vector<double> labels;
    for (i = 0; i < layers[0]; i++) {
        sample.push_back(1.0);
    }
    for (i = 0; i < layers[layers.size() - 1]; i++) {
        labels.push_back(0.1);
    }
    labels[0] = 0.9;

    gradient_check(net, sample, labels);

    delete net;
    return 0;
}
