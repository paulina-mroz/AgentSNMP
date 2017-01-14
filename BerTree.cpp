#include <vector>
#include <string>
#include <map>
#include "defines.h"
#include "BerTree.h"

BerTree::BerTree() {
}

BerTree::~BerTree() {
}

void BerTree::print_tree(int n) {
    printf("BER TREE %ld\n", sub.size());
    if (!sub.empty()) {
        for (auto &node : sub) {
            printf("\t%02d ", n);
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
    for (auto &p : sub) {
        if (p->sub.empty()) {
            delete p;
        } else {
            p->delete_tree();
        }
    }
    sub.clear();
}
