#include <stdio.h>
#include <iostream>
// #include <string>
// #include <vector>
// #include <list>

#include "Type.h"
#include "defines.h"

Type::Type() {
    DEBUG("Constructor");
    size = -1;
}

Type::~Type() {
    DEBUG("Deconstructor");
}
