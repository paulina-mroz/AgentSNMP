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
    std::vector<int> size;
    std::vector<int> range;
    std::map<int, std::string> enumInt;
    std::map<std::string, Type> table;

    std::string name;
    char ber;
};




#endif /* TYPE_H */
