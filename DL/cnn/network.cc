#include <stdio.h>

#include <vector>

#include "types.h"
#include "node.h"
#include "connection.h"
#include "layer.h"
#include "network.h"

Network::Network(const std::vector<int> &layers) {
    size_t i, j, k;

    _rate = 0.5; 

    /* 创建 各层 网络节点 */
    for (i = 0; i < layers.size(); i++) {
        Layer *l = new Layer(i, layers[i]);
        _layers.push_back(l);
    }

    /* 创建各层之间的连接 */
    for (i = 1; i < _layers.size(); i++) {

        for(j = 0; j < _layers[i-1]->_nodes.size(); j++) {
            Node *from = _layers[i-1]->_nodes[j];

            /* 每层的最后一个节点是常量节点，不需要输入 */
            for (k = 0; k < _layers[i]->_nodes.size() - 1; k++) {
                Node *to         = _layers[i]->_nodes[k];
                Connection *conn = new Connection(from, to);
                
                /* 节点中记录自己的连接*/
                from->append_downstream_conn(conn);
                to->append_upstream_conn(conn);

                _conns.push_back(conn);
            }
        }
    }
}

Network::~Network(void) {
    size_t i;
    for (i = 0; i < _layers.size(); i++) {
        delete _layers[i];
    }
    for (i = 0; i < _conns.size(); i++) {
        delete _conns[i];
    }
}

int 
Network::train(double rate, int round, 
              const std::vector<Sample *> &samples, 
              const std::vector<Label *> &labels) {
    set_rate(rate);
    for (int i = 0; i < round; i++) {
        for (size_t j = 0; j < samples.size(); j++) {
            train_one_sample(*samples[j],  *labels[j]);
        }
    }
    return 0;
}

void 
Network::update_weight(void) {
    for (size_t i = 0; i < _layers.size() - 1; i++) {
        for (size_t j = 0; j < _layers[i]->_nodes.size(); j++) {
            Node *from = _layers[i]->_nodes[j];
            for (size_t k = 0; k < from->_downstream.size(); k++) {
                Connection *conn = from->_downstream[k];
                Node       *to   = conn->_downstream_node;
                conn->_weight += _rate * to->_delta * from->_output;

                /* debug */
                assert(from == conn->_upstream_node);
            }
        }
    }
}

void 
Network::calc_gradient(void) {
    for (size_t i = 0; i+1 < _layers.size(); i++) {
        for (size_t j = 0; j < _layers[i]->_nodes.size(); j++) {
            Node *node = _layers[i]->_nodes[j];
            for (size_t k = 0; k < node->_downstream.size(); k++) {
                Connection *conn = node->_downstream[k];
                conn->calc_gradient();
            }
        }
    }
}

int 
Network::get_output(std::vector<double> &output) {
    output.clear();
    Layer *output_layer = _layers[_layers.size() - 1];

    for (size_t i = 0; i+1 < output_layer->_nodes.size(); i++) {
        Node *node = output_layer->_nodes[i];
        output.push_back(node->_output);
    }
    return 0;
}
