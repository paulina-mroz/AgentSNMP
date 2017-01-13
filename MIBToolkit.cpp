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
        v.valueStr = "DESCRIPTION TesT";
        tree.node.at(index).value.push_back(v);
    }

    index = tree.findNode("sysContact"); //RW
    if (index > -1) {
        Value v;
        v.id.push_back(0);
        v.valueStr = "contact@example";
        tree.node.at(index).value.push_back(v);
        Value v2;
        v2.id.push_back(2);
        v2.valueStr = "contact@example";
        tree.node.at(index).value.push_back(v2);
        tree.node.at(index).oid.back() = 4000;
    }

    index = tree.findNode("sysName");
    if (index > -1) {
        Value v;
        v.id.push_back(0);
        v.valueStr = "access changed for testing";
        tree.node.at(index).value.push_back(v);
        tree.node.at(index).access = "not-accessible";
    }

}

void MIBToolkit::setHardcodedTable(Tree &tree) {

}
