#ifndef TREE_H
#define TREE_H

#include <string>
#include <vector>
#include <map>
#include "defines.h"

class Node {
public:
    Node();
    ~Node();

    std::vector<int> oid;
    std::vector<int> child;
    std::string name;
};

class Tree {
public:
    Tree();
    ~Tree();
    void print();
    void print_vector(std::vector<int> v);

    std::vector<int> root;
    std::map<std::vector<int>, Node> node;
};



#endif /* TREE_H */
