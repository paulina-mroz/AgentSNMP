#include <iostream>
#include <vector>
#include <string>
#include <map>
#include "defines.h"
#include "BerTree.h"

BerTree::BerTree() {
}

BerTree::~BerTree() {
}

void BerTree::printTree() {
    std::vector<int> level;
    printNode(0, level);
}

void BerTree::printNode(int n, std::vector<int> &printLevel) {
    if (n == 0) {
        printLevel.clear();
    }
    while (printLevel.size() <= n) {
        printLevel.push_back(0);
    }
    if (!sub.empty()) {
        printLevel.at(n) += sub.size();
        for (auto &node : sub) {
            if (n == 0) {
                printf("");
            } else {
                for(int i = 1; i < n; ++i) {
                    if (printLevel.at(i)) {
                        printf("\u2502  ");
                    } else {
                        printf("   ");
                    }
                }
                if (printLevel.at(n) == 1) {
                    printf("\u2514\u2500 ");
                } else {
                    printf("\u251C\u2500 ");
                }
            }

            std::string printStr;
            if (n == 0) {
                if (printLevel.at(n) == 1) {
                    printStr = "SNMP Message";
                }
            } else if (n == 1) {
                if (printLevel.at(n) == 3) {
                    printStr = "SNMP Version";
                } else if (printLevel.at(n) == 2) {
                    printStr = "SNMP Community String";
                } else if (printLevel.at(n) == 1) {
                    printStr = "SNMP PDU";
                }
            } else if (n == 2) {
                if (printLevel.at(n) == 4) {
                    printStr = "Request ID";
                } else if (printLevel.at(n) == 3) {
                    printStr = "Error";
                } else if (printLevel.at(n) == 2) {
                    printStr = "Error Index";
                } else if (printLevel.at(n) == 1) {
                    printStr = "Varbind List";
                }
            } else if (n == 3) {
                printStr = "Varbind";
            } else if (n == 4) {
                if (printLevel.at(n) == 2) {
                    printStr = "Object Identifier";
                } else if (printLevel.at(n) == 1) {
                    printStr = "Value";
                }
            }
            std::cout << printStr << " ";
            for (int k = 0; k < (31-(printStr.size()+3*n)); ++k) {
                std::cout << "\u2504";
            }
            std::cout << " ";

            printf("T: %02X ", (unsigned char)node->type);
            printf("L: ");
            for (auto &l : node->length) {
                printf("%02X ", (unsigned char)l);
            }
            if (node->sub.empty()) {
                printf("V: ");
                for (auto &c : node->content) {
                    printf("%02X ", (unsigned char)c);
                }
            }
            printf("\n");
            printLevel.at(n)--;
            node->printNode(n+1, printLevel);

        }
    }
}

void BerTree::deleteTree() {
    for (auto &p : sub) {
        if (p->sub.empty()) {
            delete p;
        } else {
            p->deleteTree();
        }
    }
    sub.clear();
}
