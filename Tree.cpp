#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <list>

#include <boost/algorithm/string/join.hpp>

#include "Tree.h"
#include "defines.h"

Node::Node() {
    DEBUG("Constructor");
    child.clear();
    oid.clear();
    name.clear();
    // name = "fff";
}

Node::~Node() {
    DEBUG("Deconstructor");
}

bool Node::compareNode(std::string const& ref) {
    return name == ref;
}

bool Node::compareNode(std::vector<int> const& ref) {
    bool isEqual = false;
    if ( ref.size() != oid.size() ) {
        return isEqual;
    } else {
        return std::equal ( ref.begin(), ref.end(), oid.begin() );
    }
}

int Node::findValue(std::list<int> const& ref) {
    for (int i = 0; i < value.size(); ++i) {
        if (value.at(i).compareNode(ref)) {
            return i;
        }
    }
    return -1;
}

int Node::findChild(int ref) {
    for (int i = 0; i < child.size(); ++i) {
        if (child.at(i) == ref) {
            return i;
        }
    }
    return -1;
}

void Node::print_info() {
    std::cout << "NAME\n\t" << name << std::endl;
    std::cout << "OID\n\t";
    for (auto &v : oid) {
        printf("%d.", v);
    }
    std::cout << std::endl;
    if (!syntax.empty())
        std::cout << "SYNTAX\n\t" << syntax << std::endl;
    if (!access.empty())
        std::cout << "ACCESS\n\t" << access << std::endl;
    if (!status.empty())
        std::cout << "STATUS\n\t" << status << std::endl;
    if (!description.empty())
        std::cout << "DESCRIPTION\n\t" << description << std::endl;
    if (!value.empty()) {
        std::cout << "VALUES\n\t";
        for (auto &v : value) {
            for (auto &vid : v.id) {
                printf("%d.", vid);
            }
            if (type.storage == STORAGE_INT) {
                std::cout << " " << v.valueInt << std::endl;
            } else if (type.storage == STORAGE_STR) {
                std::cout << " " << v.valueStr << std::endl;
            } else if ((type.storage == STORAGE_OID) || (type.storage == STORAGE_IP)) {
                for (auto &val : v.valueOidIp) {
                    printf("%d.", val);
                }
            }
        }
    }
}


Tree::Tree() {
    DEBUG("Constructor");
    // std::vector<int> newID;
    // newID.push_back(1);
    // std::vector<int> children;
    // children.push_back(3);
    // root = newID;
    //
    // node.push_back(Node());
    // node.back().child = children;
    // node.back().name = "isoiso";
    // node.back().oid = newID;
    //
    // newID.push_back(3);
    // children.clear();
    // children.push_back(1);
    // children.push_back(2);
    // node.push_back(Node());
    // node.back().child = children;
    // node.back().oid = newID;
    //
    // newID.push_back(2);
    // node.push_back(Node());
    // node.back().oid = newID;
    // newID.pop_back();
    // newID.push_back(1);
    // children.clear();
    // children.push_back(1);
    // children.push_back(4);
    // node.push_back(Node());
    // node.back().child = children;
    // node.back().oid = newID;
    //
    // newID.push_back(1);
    // node.push_back(Node());
    // node.back().oid = newID;
    // newID.pop_back();
    // newID.push_back(4);
    // node.push_back(Node());
    // node.back().oid = newID;
}

Tree::~Tree() {
    DEBUG("Deconstructor");
}

int Tree::findNode(std::string const& ref) {
    for (int i = 0; i < node.size(); ++i) {
        if (node.at(i).compareNode(ref)) {
            return i;
        }
    }
    return -1;
}

int Tree::findNode(std::vector<int> const& ref) {
    for (int i = 0; i < node.size(); ++i) {
        if (node.at(i).compareNode(ref)) {
            return i;
        }
    }
    return -1;
}



// std::vector<Node>::iterator Tree::findNodeIterator(std::vector<int> const& ref) {
//
// }

void Tree::print_tree() {
    std::list<std::vector<int>> remainID;
    remainID.push_front(root);

    std::vector<int> id;
    std::vector<int> level;
    level.push_back(1);

    while(!remainID.empty()) {
        id = remainID.front();
        remainID.pop_front();
        int sizeID = id.size();
        if (level.size() == sizeID) {
            level.push_back(0);
        }

        int ind = findNode(id);
        if (ind >= 0) {
            for (std::vector<int>::reverse_iterator it=node.at(ind).child.rbegin(); it<node.at(ind).child.rend(); ++it) {
                std::vector<int> v = id;
                v.push_back(*it);
                remainID.push_front(v);
                level.at(sizeID)++;
            }

            if (sizeID == 1) {
                printf("");
            } else {
                for(int i = 0; i < sizeID-2; ++i) {
                    if (level.at(i+1)) {
                        printf("\u2502  ");
                    } else {
                        printf("   ");
                    }
                }
                if (level.at(sizeID-1) == 1) {
                    printf("\u2514\u2500 ");
                } else {
                    printf("\u251C\u2500 ");
                }
            }
            print_vector(id);
            std::cout << " " << node.at(ind).name << std::endl;
            level.at(sizeID-1)--;
        }
    }
}

void Tree::print_node(std::string const& name) {
    std::cout << "INFO about: " << name << std::endl;
    if (findNode(name) < 0) return;
    node.at(findNode(name)).print_info();
}

void Tree::print_node(std::vector<int> const& name) {
    std::cout << "INFO about: ";
    print_vector(name);
    std::cout << std::endl;
    if (findNode(name) < 0) return;
    node.at(findNode(name)).print_info();
}

void Tree::print_vector(std::vector<int> v) {
    for (std::vector<int>::iterator it=v.begin(); it<v.end(); ++it) {
        printf("%d.", *it);
    }
}
