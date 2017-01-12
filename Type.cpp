#include <stdio.h>
#include <iostream>
// #include <string>
// #include <vector>
// #include <list>

#include "Type.h"
#include "defines.h"

Type::Type() {
    DEBUG("Constructor");
    ber = 0;
    storage = STORAGE_NONE;
}

Type::~Type() {
    DEBUG("Deconstructor");
}

void Type::print_info() {
    std::cout << "\tPrimary type: " << primaryType << std::endl;
    std::cout << "\tStorage: " << storage << std::endl;
    std::cout << "\tBER: ";
    printf("%02X\n", (unsigned char) ber);
    std::cout << "\tSize: ";
    for (std::vector<long>::iterator it=size.begin(); it<size.end(); ++it) {
        std::cout << *it << " - " << *(it+1) << " ";
        ++it;
    }
    std::cout << std::endl;
    std::cout << "\tRange: ";
    for (std::vector<long>::iterator it=range.begin(); it<range.end(); ++it) {
        std::cout << *it << " - " << *(it+1) << " ";
        ++it;
    }
    std::cout << std::endl;
    if (!enumInts.empty()) {
        std::cout << "\tInteger Enum: ";
        for (std::vector<enumInt>::iterator it=enumInts.begin(); it<enumInts.end(); ++it) {
            std::cout << "| " << (*it).n << ": " << (*it).s << " ";
        }
        std::cout << std::endl;
    }
    // std::cout << "Enumerate: " << std::endl;;
    // for (auto &p : enumInt) {
    //     std::cout << "\t" << p.second << "(" << p.first << ")" << std::endl;
    // }
    // std::cout << "Table: " << std::endl;;
    // for (auto &p : table) {
    //     std::cout << "\t" << p.first << ": " << p.second.primaryType << std::endl;
    // }
}
