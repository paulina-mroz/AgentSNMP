#include <stdio.h>
#include <cstring>
#include <iostream>

#include "Value.h"
#include "defines.h"

Value::Value(){
    valueInt = 0;
}

Value::~Value(){
}

bool Value::compareNode(std::list<long> const& ref) {
    bool isEqual = false;
    if ( ref.size() != id.size() ) {
        return isEqual;
    } else {
        return std::equal ( ref.begin(), ref.end(), id.begin() );
    }
}
