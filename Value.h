#ifndef VALUE_H
#define VALUE_H

#include <vector>
#include <string>
#include <map>
#include <list>
#include "defines.h"

class Value {
public:
    Value();
    ~Value();

    bool compareNode(std::list<int> const& ref);

    std::list<int> id;
    std::string value;
};




#endif /* VALUE_H */
