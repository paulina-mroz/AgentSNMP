#ifndef TREE_H
#define TREE_H

#include <string>
#include <vector>
#include <map>
#include <list>
#include "defines.h"
#include "Type.h"
#include "Value.h"

class Node {
public:
    Node();
    ~Node();
    bool compareNode(std::string const& ref);
    bool compareNode(std::vector<long> const& ref);
    int findValue(std::list<long> const& ref);
    int findChild(long ref);
    int findNextChild(long ref);
    void printInfo();

    std::vector<long> oid;
    std::vector<long> child;
    std::map<long, std::vector<Node>::iterator> childp;
    std::string name;

    std::string syntax;
    std::string access;
    std::string status;
    std::string description;
    std::vector<std::string> index;


    Type type;
    std::vector<Value> value;
    std::vector<long> indexIndex;
};

class Tree {
public:
    Tree();
    ~Tree();
    int findNode(std::string const& ref);
    int findNode(std::vector<long> const& ref);
    void printTree();
    void printVector(std::vector<long> v);

    std::vector<long> root;
    std::vector<Node> node;
};

#endif /* TREE_H */
