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

    std::string primaryType;
    int size;
    std::vector<int> range;
    std::map<int, std::string> enumInt;
    // std::map<std::string, Type> table;
};

// std::vector<Type> types;


#endif /* TYPE_H */
