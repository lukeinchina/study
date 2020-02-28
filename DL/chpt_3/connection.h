#ifndef DL_CONNECTION_H_
#define DL_CONNECTION_H_

#include <stdio.h>
#include <stdlib.h>

#include "node.h"

struct Connection {
    Node *_upstream_node;
    Node *_downstream_node;
    double _weight;
    double _gradient;
    
    Connection(Node *up, Node *down) {
        _upstream_node   = up;
        _downstream_node = down;

        _weight   = (random()%20 - 10) * 0.01 ;
        _gradient = 0.0;
    }

    void print(FILE *fp = stdout) {
        fprintf(fp, "(%d,%d)-->(%d,%d) = %f\n",
                _upstream_node->_layer_index, _upstream_node->_node_index,
                _downstream_node->_layer_index, _downstream_node->_node_index,
                _weight);
    }
    
    /* debug routin */
    void calc_gradient() {
        _gradient = _downstream_node->_delta * _upstream_node->_output;
    }
    double get_gradient() {
        return _gradient;
    }
};

#endif
