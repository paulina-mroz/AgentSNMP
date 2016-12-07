#include <stdio.h>
#include <iostream>
// #include <string>
// #include <vector>
// #include <list>

#include "Type.h"
#include "defines.h"

Type::Type() {
    DEBUG("Constructor");
}

Type::~Type() {
    DEBUG("Deconstructor");
}

void Type::print_info() {
    std::cout << "Primary type: " << primaryType << std::endl;
    std::cout << "Size: ";
    for (std::vector<int>::iterator it=size.begin(); it<size.end(); ++it) {
        std::cout << *it << " - " << *(++it) << " ";
    }
    std::cout << std::endl;
    std::cout << "Range: ";
    for (std::vector<int>::iterator it=range.begin(); it<range.end(); ++it) {
        std::cout << *it << " - " << *(++it) << " ";
    }
    std::cout << std::endl;
    std::cout << "Enumerate: " << std::endl;;
    for (auto &p : enumInt) {
        std::cout << "\t" << p.second << "(" << p.first << ")" << std::endl;
    }
    std::cout << "Table: " << std::endl;;
    for (auto &p : table) {
        std::cout << "\t" << p.first << ": " << p.second.primaryType << std::endl;
    }
}
