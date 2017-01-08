#include <stdio.h>
#include <cstring>
#include <iostream>

#include "Value.h"
#include "defines.h"

Value::Value(){
    DEBUG("Constructor\n");
}

Value::~Value(){
    DEBUG("Deconstructor\n");
}

bool Value::compareNode(std::list<int> const& ref) {
    bool isEqual = false;
    if ( ref.size() != id.size() ) {
        return isEqual;
    } else {
        return std::equal ( ref.begin(), ref.end(), id.begin() );
    }
}
