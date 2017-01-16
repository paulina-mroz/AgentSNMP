#ifndef TYPE_H
#define TYPE_H

#include <vector>
#include <string>
#include <map>
#include "defines.h"

class Type {
public:
    Type();
    ~Type();
    void printInfo();

    std::string primaryType;
    std::vector<long> size;
    std::vector<long> range;
    struct enumInt {
       long n;
       std::string s;
    };
    std::vector<enumInt> enumInts;

    char ber;
    int storage;
};

#endif /* TYPE_H */
