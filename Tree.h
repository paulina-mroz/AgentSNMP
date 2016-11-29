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
    bool compareNode(std::string const& ref);
    bool compareNode(std::vector<int> const& ref);

    std::vector<int> oid;
    std::vector<int> child;
    std::string name;

    std::string syntax;
    std::string access;
    std::string status;
    std::string description;
    std::vector<std::string> index;
};

class Tree {
public:
    Tree();
    ~Tree();
    int findNode(std::string const& ref);
    int findNode(std::vector<int> const& ref);
    void print_tree();
    void print_node(std::string name);
    void print_vector(std::vector<int> v);


    std::vector<int> root;
    //std::map<std::vector<int>, Node> node;
    std::vector<Node> node;
};



#endif /* TREE_H */
