#!/usr/bin/env python

my_data = [
        ['slashdot', 'USA', 'yes', 18, 'None'],
        ['google', 'France', 'yes', 23, 'Premium'],
        ['digg', 'USA', 'yes', 24, 'Basic'],
        ['kiwitobes', 'France', 'yes', 23, 'Basic'],
        ['google', 'UK', 'no', 21, 'Premium'],
        ['(direct)', 'New Zealand', 'no', 12, 'None'],
        ['(direct)', 'UK', 'no', 21, 'Basic'],
        ['google', 'USA', 'no', 24, 'Premium'],
        ['slashdot', 'France', 'yes', 19, 'None'],
        ['digg', 'USA', 'no', 18, 'None'],
        ['google', 'UK', 'no', 18, 'None'],
        ['kiwitobes', 'UK', 'no', 19, 'None'],
        ['digg', 'New Zealand', 'yes', 12, 'Basic'],
        ['slashdot', 'UK', 'no', 21, 'None'],
        ['google', 'UK', 'yes', 18, 'Basic'],
        ['kiwitobes', 'France', 'yes', 19, 'Basic']
        ]

class DecisionNode:
    def __init__(self, col=-1, value=None, results=None, tb=None, fb=None):
        self.col = col
        self.value = value
        self.results = results
        self.tb = tb
        self.fb = fb

#
#
def divide_set(rows, column, value):
    #
    #
    split_func = None
    if isinstance(value, int) or isinstance(value, float):
        split_func = lambda row: row[column] >= value
    else:
        split_func = lambda row: row[column] == value
    #
    set1 = [row for row in rows if split_func(row)]
    set2 = [row for row in rows if not split_func(row)]
    return (set1, set2)

#
#
def unique_counts(rows):
    results = {}
    for row in rows:
        r = row[-1]
        if r not in results: results[r] = 0
        results[r] += 1
    return results

#
#
def entropy(rows):
    from math import log
    log2 = lambda x: log(x)/log(2)
    results = unique_counts(rows)
    ent = 0.0
    for r in results.keys():
        p = float(results[r]) / len(rows)
        ent = ent - p * log2(p)
    return ent

def build_tree(rows, score_func=entropy):
    if (len(rows) == 0):return DecisionNode()
    current_score = score_func(rows)

    # set up some
    best_gain = 0
    best_criteria = None
    best_sets = None

    column_count = len(rows[0]) - 1
    for i in range(0, column_count):
        #
        column_values = {}
        for r in rows:
            column_values[r[i]] = 1
        #
        for value in column_values.keys():
            (set1, set2) = divide_set(rows, i, value);
            #
            p = float(len(set1)) / len(rows);
            gain = current_score - p * score_func(set1) - (1-p) * score_func(set2);
            if gain > best_gain and len(set1) > 0 and len(set2) > 0:
                best_gain = gain
                best_criteria = (i, value)
                best_sets = (set1, set2)
            #
        #
        if (best_gain > 0):
            true_branch = build_tree(best_sets[0])
            false_branch = build_tree(best_sets[1])
            return DecisionNode(col=best_criteria[0], value=best_criteria[1],
                    tb=true_branch, fb=false_branch)
        else:
            return DecisionNode(results=unique_counts(rows))
    #end for i in range

def print_tree(tree, indent=''):
    if tree.results != None:
        print str(tree.results)
    else:
        print str(tree.col)+":"+str(tree.value)+"?"

        print indent + "T->",
        print_tree(tree.tb, indent + '  ')
        print indent + "F->"
        print_tree(tree.fb, indent + '  ')
    #
#end def

if __name__ == "__main__":
    results = unique_counts(my_data)
    for k in results:
        print k, results[k]
    #
    print entropy(my_data)
    set1,set2 = divide_set(my_data, 2, 'yes')
    print entropy(set1)
    print entropy(set2)
    tree = build_tree(my_data)
    print_tree(tree)
