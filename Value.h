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

    bool compareNode(std::list<long> const& ref);

    std::list<long> id;
    std::string valueStr;
    long valueInt;
    std::vector<long> valueOidIp;
};

#endif /* VALUE_H */
