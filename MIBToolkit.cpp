#include <stdio.h>
#include <cstring>
#include <iostream>

#include "MIBToolkit.h"
#include "defines.h"

MIBToolkit::MIBToolkit(){
    DEBUG("Constructor\n");
}

MIBToolkit::~MIBToolkit(){
    DEBUG("Deconstructor\n");
}

void MIBToolkit::getPermissions(std::string communityString) {
    permissionRO = false;
    permissionRW = false;

    if (communityString == "public") {
        permissionRO = true;
    } else if (communityString == "private") {
        permissionRO = true;
        permissionRW = true;
    }
}

void MIBToolkit::makeResponsePDU(SNMPDeserializer &di, SNMPSerializer &si, Tree &tree) {
// void MIBToolkit::makeResponsePDU(BerTree &dbt, BerTree &sbt) {
    std::list<char> requestID = di.berTreeInst.sub.at(0)->sub.at(2)->sub.at(0)->content;

    si.berTreeInst.sub.at(0)->sub.push_back(new BerTree());
    si.berTreeInst.sub.at(0)->sub.at(2)->type = typeMap["GETRESPONSE"].ber;
    si.berTreeInst.sub.at(0)->sub.at(2)->sub.push_back(new BerTree());
    si.berTreeInst.sub.at(0)->sub.at(2)->sub.at(0)->type = typeMap["INTEGER"].ber;
    for (auto &i : requestID) {
        si.berTreeInst.sub.at(0)->sub.at(2)->sub.at(0)->content.push_back(i);
    }
    si.berTreeInst.sub.at(0)->sub.at(2)->sub.push_back(new BerTree());
    si.berTreeInst.sub.at(0)->sub.at(2)->sub.at(1)->type = typeMap["INTEGER"].ber;
    si.berTreeInst.sub.at(0)->sub.at(2)->sub.at(1)->content.push_back(0);
    si.berTreeInst.sub.at(0)->sub.at(2)->sub.push_back(new BerTree());
    si.berTreeInst.sub.at(0)->sub.at(2)->sub.at(2)->type = typeMap["INTEGER"].ber;
    si.berTreeInst.sub.at(0)->sub.at(2)->sub.at(2)->content.push_back(0);

    si.berTreeInst.sub.at(0)->sub.at(2)->sub.push_back(new BerTree());
    si.berTreeInst.sub.at(0)->sub.at(2)->sub.at(3)->type = typeMap["SEQUENCE"].ber;

    si.assignBerTreeLength(si.berTreeInst);

    for (auto &varbind : di.berTreeInst.sub.at(0)->sub.at(2)->sub.at(3)->sub) {
        printf("VARBIND\n");
        std::vector<int> oid = di.getOidValue(varbind->sub.at(0)->content);
        std::list<int> valueList;
        int oidIndex = -1;
        int valueIndex = -1;

        for (auto &p : oid) {
            printf("oid %d ", p);
        }
        while (oid.size() && (oidIndex < 0)) {
            oidIndex = tree.findNode(oid);
            printf("oid %d ", oidIndex);
            if (oidIndex < 0) {
                valueList.push_front(oid.back());
                printf("vl %d ", valueList.front());
                oid.pop_back();
            }
        }


    }

}
