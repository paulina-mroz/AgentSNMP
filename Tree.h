#ifndef TREE_H
#define TREE_H

#include <string>
#include <vector>
#include <map>
#include "defines.h"
#include "Type.h"

class Node {
public:
    Node();
    ~Node();
    bool compareNode(std::string const& ref);
    bool compareNode(std::vector<int> const& ref);

    std::vector<int> oid;
    std::vector<int> child;
    std::map<int, std::vector<Node>::iterator> childp;
    std::string name;

    // std::string syntax;
    Type syntax;
    std::string access;
    std::string status;
    std::string description;
    std::vector<std::string> index;
    std::map<std::vector<int>, std::string> value;
};

class Tree {
public:
    Tree();
    ~Tree();
    int findNode(std::string const& ref);
    int findNode(std::vector<int> const& ref);
    void print_tree();
    void print_node(std::string const& name);
    void print_node(std::vector<int> const& name);
    void print_vector(std::vector<int> v);


    std::vector<int> root;
    //std::map<std::vector<int>, Node> node;
    std::vector<Node> node;
};



#endif /* TREE_H */
