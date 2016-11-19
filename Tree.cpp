#include <stdio.h>
#include <vector>
#include <list>

#include "Tree.h"
#include "defines.h"

Node::Node() {
    DEBUG("Constructor\n");
    child.clear();
    oid.clear();
    //name.clear();
    name = "fff";
}

Node::~Node() {
    DEBUG("Deconstructor\n");
}

Tree::Tree() {
    DEBUG("Constructor\n");
    std::vector<int> newID;
    newID.push_back(1);
    std::vector<int> children;
    children.push_back(3);
    root = newID;

    node[newID] = Node();
    node.at(newID).child = children;
    node.at(newID).name = "isoiso";

    newID.push_back(3);
    children.clear();
    children.push_back(1);
    children.push_back(2);
    node[newID] = Node();
    node.at(newID).child = children;

    newID.push_back(2);
    node[newID] = Node();
    newID.pop_back();
    newID.push_back(1);
    children.clear();
    children.push_back(1);
    node[newID] = Node();
    node.at(newID).child = children;

    newID.push_back(1);
    node[newID] = Node();
    DEBUG("Size: %d", node.at(newID).child.size());
}

Tree::~Tree() {
    DEBUG("Deconstructor\n");
}

void Tree::print() {
    std::list<std::vector<int>> remainID;
    remainID.push_front(root);

    std::vector<int> id;
    while(!remainID.empty()) {
        id = remainID.front();
        remainID.pop_front();
        int sizeID = id.size();

        for (std::vector<int>::reverse_iterator it=node.at(id).child.rbegin(); it<node.at(id).child.rend(); ++it) {
            std::vector<int> v = id;
            v.push_back(*it);
            remainID.push_front(v);
        }

        if (sizeID == 1) {
            printf("+ ");
        } else {
            for(int i = 0; i < sizeID-2; ++i) {
                printf("| ");
            }
            printf("+-+ ");
        }
        //printf("%s\n",node.at(id).name);
    }
}

void Tree::print_vector(std::vector<int> v) {
    printf("vector: ");
    for (std::vector<int>::iterator it=v.begin(); it<v.end(); ++it) {
        printf("%d ", *it);
    }
    printf("\n");
}
