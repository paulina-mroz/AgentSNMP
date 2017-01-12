#include <stdio.h>
#include <cstring>
#include <iostream>

#include "MIBToolkit.h"
#include "Value.h"
#include "defines.h"

MIBToolkit::MIBToolkit(){
    DEBUG("Constructor\n");
}

MIBToolkit::~MIBToolkit(){
    DEBUG("Deconstructor\n");
}

void MIBToolkit::setHardcodedValues(Tree &tree) {
    int index = -1;
    index = tree.findNode("sysDescr"); //RO
    if (index > -1) {
        Value v;
        v.id.push_back(0);
        v.value = "DESCRIPTION TesT";
        tree.node.at(index).value.push_back(v);
    }

    index = tree.findNode("sysContact"); //RW
    if (index > -1) {
        Value v;
        v.id.push_back(0);
        v.value = "contact@example";
        tree.node.at(index).value.push_back(v);
        // tree.node.at(index).oid.back() = 4000;
    }

    index = tree.findNode("sysName");
    if (index > -1) {
        Value v;
        v.id.push_back(0);
        v.value = "access changed for testing";
        tree.node.at(index).value.push_back(v);
        tree.node.at(index).access = "not-accessible";
    }
}
