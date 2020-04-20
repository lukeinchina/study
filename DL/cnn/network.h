/*
 *
 *
 * 本实现参考自:https://www.zybuluo.com/hanbingtao/note/476663
 *
 */

#ifndef DL_NETWORK_H_
#define DL_NETWORK_H_

#include "types.h"
#include "connection.h"
#include "layer.h"

struct Network {
    std::vector<Layer *>      _layers;
    std::vector<Connection *> _conns;
    double _rate;

    std::vector<double>       _outputs;

    /* 用数组的值初始化每层的节点数 */
    Network(const std::vector<int> &layers);
    ~Network(void);

    void set_rate(double rate) {
        _rate = rate;
    }

    int train(double rate, int round, 
              const std::vector<Sample *> &samples, 
              const std::vector<Label *> &labels);

    int train_one_sample(const std::vector<double> &sample, 
            const std::vector<double> &labels){
        int i = _layers.size() - 1;
        if (_layers.size() < 2 
                || sample.size() + 1 != _layers[0]->_nodes.size()
                || labels.size() + 1 != _layers[i]->_nodes.size()) {
            return -1;
        }

        predict(sample);
        calc_delta(labels);
        update_weight();
        return 0;
    }

    void update_weight(void);

    void predict(const std::vector<double> &sample) {
        /*  输入层 */
        _layers[0]->set_output(sample);

        /* 隐藏层和输出层 */
        for (size_t i = 1; i < _layers.size(); i++) {
            _layers[i]->calc_output();
        }
    }

    int get_output(std::vector<double> &output);

    void calc_delta(const std::vector<double> &labels) {
        int i;
        size_t j;
        i = _layers.size() - 1;

        /* 输出层 */
        for (j = 0; j < labels.size(); j++) {
            _layers[i]->_nodes[j]->calc_output_layer_delta(labels[j]);
        }

        /* 隐藏层 */
        for (i -= 1; i >= 0; i--) {
            /* 最后一个节点是常量节点 ? */
            for (j = 0; j < _layers[i]->_nodes.size(); j++) {
                _layers[i]->_nodes[j]->calc_hidden_layer_delta();
            }
        }
    }

    void dump(FILE *fp = stdout) {
        for (size_t i = 0; i < _layers.size(); i++) {
            _layers[i]->print(fp);
            fprintf(fp, "\033[32m-------END layer[%lu]----\n\n\033[0m", i+1);
        }
    }

    /* debug */
    void calc_gradient(void);

    void get_gradient(const std::vector<double> &sample, 
            const std::vector<double> &labels){ 
        predict(sample);
        calc_delta(labels);
        calc_gradient();
    }
            
};

#endif
