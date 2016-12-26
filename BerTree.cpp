#include <vector>
#include <string>
#include <map>
#include "defines.h"
#include "BerTree.h"


BerTree::BerTree() {
    DEBUG("Constructor");
}

BerTree::~BerTree() {
    DEBUG("Deconstructor");
}

void BerTree::print_tree(int n) {
    printf("BER TREE\n");
    if (!sub.empty()) {
        for (auto &node : sub) {
            printf("%02d ", n);
            printf("T: %02X ", (unsigned char)node->type);
            printf("L: ");
            for (auto &l : node->length) {
                printf("%02X ", (unsigned char)l);
            }
            printf("C: ");
            for (auto &c : node->content) {
                printf("%02X ", (unsigned char)c);
            }
            printf("\n");
            node->print_tree(n+1);
        }
    }
}

void BerTree::delete_tree() {
    // for (std::vector<BerTree>::iterator it=sub.begin(); it<sub.end(); ++it) {
    //     delete *it;
    // }
    printf("DELETE: SIZE B: %d\n", sub.size());
    for (auto &p : sub) {
        if (p->sub.empty()) {
            delete p;
        } else {
            p->delete_tree();
        }
    }
    sub.clear();
    printf("DELETE: SIZE A: %d\n", sub.size());
}
