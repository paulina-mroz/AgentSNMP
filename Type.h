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
    void print_info();

    std::string primaryType;
    std::vector<long> size;
    std::vector<long> range;
    struct enumInt {
       int n;
       std::string s;
    };
    std::vector<enumInt> enumInts;
    // std::map<int, std::string> enumInt;
    // std::map<std::string, Type> table;

    // std::string name;
    char ber;
};




#endif /* TYPE_H */
