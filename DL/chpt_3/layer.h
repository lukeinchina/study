#ifndef DL_LAYER_H_
#define DL_LAYER_H_

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <vector>

#include "node.h"

struct Layer {
    int _layer_index;
    std::vector<Node *> _nodes;

    Layer(int layer_idx,  int node_count) {
        _layer_index = layer_idx;
        for (int i = 0; i < node_count; i++) {
            Node *node = new Node(layer_idx, i);
            assert(NULL != node);
            _nodes.push_back(node);
        }

        /* 加一个常量节点 */
        ConstNode *node = new ConstNode(_layer_index, node_count);
        _nodes.push_back(node);
    }

    void calc_output(void) {
        for (size_t i = 0; i < _nodes.size() - 1; i++) {
            _nodes[i]->calc_output();
        }
        /* 常量节点不需要计算 */
    }

    /* 第一层设置输入样本 */
    void set_output(const std::vector<double> &samples) {
        assert(_nodes.size() == samples.size()+1);
        for (size_t i = 0; i < samples.size(); i++) {
            _nodes[i]->set_output(samples[i]);
        }
        /* 常量节点不需要 */
    }
    
    void print(FILE *fp = stdout) {
        fprintf(fp, "layer index: %d, node count %lu\n",
                _layer_index, _nodes.size());
        for (size_t i = 0; i < _nodes.size(); i++) {
            _nodes[i]->print(fp);
        }
    }
};

#endif
