/*
 * node节点类
 *
 * 理论和实现参考来源: https://www.zybuluo.com/hanbingtao/note/476663
 */
#ifndef DL_NODE_H_
#define DL_NODE_H_

#include <stdio.h>

#include <vector>

/* 类声明 */
struct Connection;

struct Node {
    int                       _layer_index; /* 网络层次编号 */
    int                       _node_index;  /* 节点编号     */
    double                    _output;      /* 输出值       */
    double                    _delta;       /* 误差项       */
    std::vector<Connection *> _downstream;  /* 下游连接     */
    std::vector<Connection *> _upstream;    /* 上游连接     */

    Node(int layer_idx, int node_idx) {
        _layer_index = layer_idx;
        _node_index  = node_idx;
        _output      = 0.0;
        _delta       = 0.0;
    }

    void append_downstream_conn(Connection *conn) {
        _downstream.push_back(conn);
    }

    void append_upstream_conn(Connection *conn) {
        _upstream.push_back(conn);
    }

    void calc_hidden_layer_delta(void) ;

    void calc_output_layer_delta(double label) {
        _delta = _output * (1 - _output) * (label - _output);
    }

    /* 输入层会用到 */
    virtual void set_output(double val) {
        _output = val;
    }

    virtual void calc_output(void);

    /* 信息输出 for debug */
    virtual void print(FILE *fp = stdout);

    virtual ~Node(void) {
        ;
    }
};

/*
 * ConstNode，为了实现一个输出恒为1的节点(计算偏置项时需要)
 *
 */
struct ConstNode : Node{

    /* 方法 */
    ConstNode(int layer_idx, int node_idx) : Node(layer_idx, node_idx) {
        /* 常量节点，输出恒为1. */
        _delta       = 0.0;
        _output      = 1.0;
        _layer_index = layer_idx;
        _node_index  = node_idx;
    }

    virtual void print(FILE *fp = stdout);
    
    virtual void set_output(double val) {
        (void)val;
        abort();
    }
    virtual void calc_output(void) {
        abort();
    }
};


#endif
