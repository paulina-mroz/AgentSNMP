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
    std::string valueStr;
    long valueInt;
    std::vector<int> valueOidIp;
};




#endif /* VALUE_H */
