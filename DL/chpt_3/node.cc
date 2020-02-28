#include <stdio.h>
#include <math.h>

#include "connection.h"
#include "node.h"

void ConstNode::print(FILE *fp){
    fprintf(fp, "\033[33mconst node [%d:%d] output : 1\n\033[0m", _layer_index, _node_index);
    fprintf(fp, "downstream connections:\n");

    for (size_t i = 0; i < _downstream.size(); i++) {
        _downstream[i]->print(fp);
    }
    return ;
}

void Node::print(FILE *fp){
    fprintf(fp, "\033[31mNode[%d:%d] output : \n\033[0m", _layer_index, _node_index);

    fprintf(fp, "downstream connections count = %lu :\n", _downstream.size());
    for (size_t i = 0; i < _downstream.size(); i++) {
        _downstream[i]->print(fp);
    }
    fprintf(fp, "upstream connections count =  %lu :\n", _upstream.size());
    for (size_t i = 0; i < _upstream.size(); i++) {
        _upstream[i]->print(fp);
    }
    return ;
}

void Node::calc_hidden_layer_delta(void) {
    double val = 0.0;

    for (size_t i = 0; i < _downstream.size(); i++) {
        Connection *conn = _downstream[i];
        Node       *node = conn->_downstream_node;
        val             += conn->_weight * node->_delta;
    }

    _delta = _output * (1 - _output) * val; 
}

void Node::calc_output(void) {
    // const static double alpha = 1.0;

    double x = 0.0;
    for (size_t i = 0; i < _upstream.size(); i++) {
        Connection *conn = _upstream[i];
        Node       *node = conn->_upstream_node;
        x               += conn->_weight * node->_output;
    }
    _output = 1.0 / (1.0 + exp(-x));
}
