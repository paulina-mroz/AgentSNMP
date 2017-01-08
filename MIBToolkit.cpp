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
    bool oidCorrect = false;
    char requestType = di.berTreeInst.sub.at(0)->sub.at(2)->type;

    requestID.clear();
    errorValue = ERROR_NOERROR;
    errorIndex = 0;
    oidList.clear();
    valueList.clear();

    requestID = di.berTreeInst.sub.at(0)->sub.at(2)->sub.at(0)->content;

    makeSkelPDU(si);
    oidCorrect = (requestType == typeMap["GETNEXTREQUEST"].ber) ? checkOidExistenceNext(di, tree) : checkOidExistence(di, tree);
    if (!oidCorrect) {
            makeWrongOidPDU(di, si);
            return;
    }
    printf("Varbind OID correct :)\n");

}

void MIBToolkit::makeSkelPDU(SNMPSerializer &si) {
    si.berTreeInst.sub.at(0)->sub.push_back(new BerTree());
    si.berTreeInst.sub.at(0)->sub.at(2)->type = typeMap["GETRESPONSE"].ber;
    si.berTreeInst.sub.at(0)->sub.at(2)->sub.push_back(new BerTree());
    si.berTreeInst.sub.at(0)->sub.at(2)->sub.at(0)->type = typeMap["INTEGER"].ber;
    for (auto &i : requestID) {
        si.berTreeInst.sub.at(0)->sub.at(2)->sub.at(0)->content.push_back(i);
    }
    si.berTreeInst.sub.at(0)->sub.at(2)->sub.push_back(new BerTree());
    si.berTreeInst.sub.at(0)->sub.at(2)->sub.at(1)->type = typeMap["INTEGER"].ber;
    // si.berTreeInst.sub.at(0)->sub.at(2)->sub.at(1)->content.push_back(0);
    si.berTreeInst.sub.at(0)->sub.at(2)->sub.push_back(new BerTree());
    si.berTreeInst.sub.at(0)->sub.at(2)->sub.at(2)->type = typeMap["INTEGER"].ber;
    // si.berTreeInst.sub.at(0)->sub.at(2)->sub.at(2)->content.push_back(0);

    si.berTreeInst.sub.at(0)->sub.at(2)->sub.push_back(new BerTree());
    si.berTreeInst.sub.at(0)->sub.at(2)->sub.at(3)->type = typeMap["SEQUENCE"].ber;

    si.assignBerTreeLength(si.berTreeInst);
}

void MIBToolkit::makeWrongOidPDU(SNMPDeserializer &di, SNMPSerializer &si) {
    si.berTreeInst.sub.at(0)->sub.at(2)->sub.at(1)->content.push_back(errorValue);
    si.berTreeInst.sub.at(0)->sub.at(2)->sub.at(2)->content.push_back(errorIndex);

    for (auto &varbind : di.berTreeInst.sub.at(0)->sub.at(2)->sub.at(3)->sub) {
        si.berTreeInst.sub.at(0)->sub.at(2)->sub.at(3)->sub.push_back(new BerTree());
        si.berTreeInst.sub.at(0)->sub.at(2)->sub.at(3)->sub.back()->type = typeMap["SEQUENCE"].ber;
        si.berTreeInst.sub.at(0)->sub.at(2)->sub.at(3)->sub.back()->sub.push_back(new BerTree());
        si.berTreeInst.sub.at(0)->sub.at(2)->sub.at(3)->sub.back()->sub.back()->type = typeMap["OBJECT IDENTIFIER"].ber;
        si.berTreeInst.sub.at(0)->sub.at(2)->sub.at(3)->sub.back()->sub.back()->content = varbind->sub.at(0)->content;
        si.berTreeInst.sub.at(0)->sub.at(2)->sub.at(3)->sub.back()->sub.push_back(new BerTree());
        si.berTreeInst.sub.at(0)->sub.at(2)->sub.at(3)->sub.back()->sub.back()->type = typeMap["NULL"].ber;
    }

    si.assignBerTreeLength(si.berTreeInst);
}


bool MIBToolkit::checkOidExistence(SNMPDeserializer &di, Tree &tree) {
    int varbindCount = 0;
    for (auto &varbind : di.berTreeInst.sub.at(0)->sub.at(2)->sub.at(3)->sub) {
        printf("VARBIND\n");
        varbindCount++;

        printf("Varbind check type %02X\n", varbind->type);
        if (varbind->type != typeMap["SEQUENCE"].ber) {
            errorIndex = varbindCount;
            errorValue = ERROR_GENERR;
            return false;
        }

        printf("Varbind check size %d\n", varbind->sub.size());
        if (varbind->sub.size() != 2) {
            errorIndex = varbindCount;
            errorValue = ERROR_GENERR;
            return false;
        }

        printf("Varbind check OID type %02X\n", varbind->sub.at(0)->type);
        if (varbind->sub.at(0)->type != typeMap["OBJECT IDENTIFIER"].ber) {
            errorIndex = varbindCount;
            errorValue = ERROR_GENERR;
            return false;
        }

        std::vector<int> oid = di.getOidValue(varbind->sub.at(0)->content);
        std::list<int> valueID;
        int oidIndex = -1;
        int valueIndex = -1;

        for (auto &p : oid) {
            printf("oid %d ", p);
        }
        while (oid.size() && (oidIndex < 0)) {
            oidIndex = tree.findNode(oid);
            printf("ind %d ", oidIndex);
            if (oidIndex < 0) {
                valueID.push_front(oid.back());
                printf("vl %d ", valueID.front());
                oid.pop_back();
            }
        }
        if (oidIndex < 0) {
            errorIndex = varbindCount;
            errorValue = ERROR_NOSUCHNAME;
            return false;
        }
        oidList.push_back(oidIndex);
        valueIndex = tree.node.at(oidIndex).findValue(valueID);
        if (valueIndex < 0) {
            errorIndex = varbindCount;
            errorValue = ERROR_NOSUCHNAME;
            return false;
        }
        valueList.push_back(valueIndex);


    }
    return true;
}

bool MIBToolkit::checkOidExistenceNext(SNMPDeserializer &di, Tree &tree) {
    int varbindCount = 0;
    for (auto &varbind : di.berTreeInst.sub.at(0)->sub.at(2)->sub.at(3)->sub) {
        printf("VARBIND NEXT\n");
        varbindCount++;

        printf("Varbind check type %02X\n", varbind->type);
        if (varbind->type != typeMap["SEQUENCE"].ber) {
            errorIndex = varbindCount;
            errorValue = ERROR_GENERR;
            return false;
        }

        printf("Varbind check size %d\n", varbind->sub.size());
        if (varbind->sub.size() != 2) {
            errorIndex = varbindCount;
            errorValue = ERROR_GENERR;
            return false;
        }

        printf("Varbind check OID type %02X\n", varbind->sub.at(0)->type);
        if (varbind->sub.at(0)->type != typeMap["OBJECT IDENTIFIER"].ber) {
            errorIndex = varbindCount;
            errorValue = ERROR_GENERR;
            return false;
        }

        std::vector<int> oid = di.getOidValue(varbind->sub.at(0)->content);
        std::list<int> valueID;
        int oidIndex = -1;
        int valueIndex = -1;

        while (oid.size() && (oidIndex < 0)) {
            oidIndex = tree.findNode(oid);
            printf("ind %d ", oidIndex);
            if (oidIndex < 0) {
                valueID.push_front(oid.back());
                printf("vl %d ", valueID.front());
                oid.pop_back();
            }
        }
        if (oidIndex < 0) {
            errorIndex = varbindCount;
            errorValue = ERROR_NOSUCHNAME;
            return false;
        }
        valueIndex = tree.node.at(oidIndex).findValue(valueID);
        if ((valueIndex < 0) || (tree.node.at(oidIndex).value.size() < (valueIndex+2))) {
            int nextIndex = -1;
            while ((oid.size() > 1) && (nextIndex < 0)) {
                int childNumber = oid.back();
                oid.pop_back();
                int parentIndex = tree.findNode(oid);
                int childIndex = tree.node.at(parentIndex).findChild(childNumber);
                if (tree.node.at(parentIndex).child.size() > (childIndex+1)) {
                    nextIndex = childIndex + 1;
                    oid.push_back(nextIndex);
                }
                printf("FFFFFFF %d %d FFFFFFFFF\n", childNumber, childIndex);
            }
            if (nextIndex < 0) {
                errorIndex = varbindCount;
                errorValue = ERROR_NOSUCHNAME;
                return false;
            }

            int childNodeIndex = tree.findNode(oid);
            int childSize = tree.node.at(childNodeIndex).child.size();
            while (childSize != 0) {
                oid.push_back(tree.node.at(childNodeIndex).child.at(0));
                childNodeIndex = tree.findNode(oid);
                childSize = tree.node.at(childNodeIndex).child.size();
            }

            oidList.push_back(childNodeIndex);
            if (tree.node.at(childNodeIndex).value.size() == 0) {
                errorIndex = varbindCount;
                errorValue = ERROR_NOSUCHNAME;
                return false;
            }
            valueList.push_back(0);

        } else {
            oidList.push_back(oidIndex);
            valueList.push_back(valueIndex+1);
        }

    }
    return true;
}
