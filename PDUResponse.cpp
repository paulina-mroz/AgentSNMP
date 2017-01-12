#include <stdio.h>
#include <cstring>
#include <iostream>
#include <fstream>
#include <regex>

#include <boost/algorithm/string/regex.hpp>
#include <boost/algorithm/string.hpp>

#include "PDUResponse.h"
#include "defines.h"

PDUResponse::PDUResponse(){
}

PDUResponse::~PDUResponse(){
}

void PDUResponse::initPermissions(std::string fileName) {
    std::ifstream myfile(fileName);
    std::string line;
    if(myfile.is_open()) {
        while (std::getline(myfile,line)) {
            std::string linePermission = line;
            boost::trim(linePermission);

            std::regex rgx("^[ ]*(r[wo]community)[ ]+([\\S]+)[ ]*([\\d]*)[\\.]*([\\d]*)[\\.]*([\\d]*)[\\.]*([\\d]*)[\\/]*([\\d]*)[ ]*$");
            std::smatch match;

            for(std::sregex_iterator i = std::sregex_iterator(linePermission.begin(), linePermission.end(), rgx); i != std::sregex_iterator(); ++i ) {
                match = *i;
                // for (unsigned i=0; i<match.size(); ++i)
                //     std::cout << "match #" << i << ": " << match[i] << std::endl;

                bool exist = false;
                for (auto &p : permissions) {
                    if (p.cs == match.str(2)) exist = true;
                }

                if (!exist) {
                    permissionStruct ps;
                    ps.cs = match.str(2);
                    if (match.str(1) == "rocommunity") {
                        ps.read = true;
                        ps.write = false;
                    } else if (match.str(1) == "rwcommunity") {
                        ps.read = true;
                        ps.write = true;
                    }
                    if ((match.str(3).empty()) || (match.str(4).empty()) || (match.str(5).empty()) || (match.str(6).empty())) {
                        ps.ip = 0;
                        ps.mask = 0;
                    } else {
                        ps.ip = 0;
                        ps.ip = ps.ip | ((std::stoi(match.str(6)) & 0xFF) << 24);
                        ps.ip = ps.ip | ((std::stoi(match.str(5)) & 0xFF) << 16);
                        ps.ip = ps.ip | ((std::stoi(match.str(4)) & 0xFF) << 8);
                        ps.ip = ps.ip | (std::stoi(match.str(3)) & 0xFF);
                        if (match.str(7).empty()) {
                            ps.mask = 0xFFFFFFFF;
                        } else {
                            int m = std::stoi(match.str(7));
                            ps.mask = 0;
                            if (m > 24) {
                                ps.mask = ps.mask | (((1<<(m-24))-1) << 24);
                                ps.mask = ps.mask | (0xFF << 16);
                                ps.mask = ps.mask | (0xFF << 8);
                                ps.mask = ps.mask | (0xFF);
                            } else if (m > 16) {
                                ps.mask = ps.mask | (((1<<(m-16))-1) << 16);
                                ps.mask = ps.mask | (0xFF << 8);
                                ps.mask = ps.mask | (0xFF);
                            } else if (m > 8) {
                                ps.mask = ps.mask | (((1<<(m-8))-1) << 8);
                                ps.mask = ps.mask | (0xFF);
                            } else if (m > 0) {
                                ps.mask = ps.mask | ((1<<(m))-1);
                            } else if (m <= 0) {
                                ps.mask = 0xFFFFFFFF;
                            }
                        }
                    }
                    permissions.push_back(ps);
                }
            }
        }
        myfile.close();
    }
}

bool PDUResponse::getPermissions(std::string communityString, unsigned long menagerIP) {
    permissionRead = false;
    permissionWrite = false;

    for (auto &p : permissions) {
        if (communityString == p.cs) {
            if ((menagerIP & p.mask) == (p.ip & p.mask)) {
                permissionRead = p.read;
                permissionWrite = p.write;
                return true;
            } else {
                return false;
            }
        }
    }
    return false;
}

void PDUResponse::makeResponsePDU(SNMPDeserializer &di, SNMPSerializer &si, Tree &tree) {
    bool correct = false;
    requestType = di.berTreeInst.sub.at(0)->sub.at(2)->type;

    requestID.clear();
    errorValue = ERROR_NOERROR;
    errorIndex = 0;
    oidList.clear();
    valueList.clear();

    requestID = di.berTreeInst.sub.at(0)->sub.at(2)->sub.at(0)->content;

    makeSkelPDU(si);
    correct = (requestType == typeMap["GETNEXTREQUEST"].ber) ? checkOidExistenceNext(di, tree) : checkOidExistence(di, tree);
    if (!correct) {
            makeWrongOidPDU(di, si);
            return;
    }
    printf("Varbind OID correct :)\n");
    correct = checkValueCorectness(tree);
    if (!correct) {
            makeWrongValuePDU(si, tree);
            return;
    }
    printf("Varbind value correct :)\n");
}

void PDUResponse::makeSkelPDU(SNMPSerializer &si) {
    si.berTreeInst.sub.at(0)->sub.push_back(new BerTree());
    si.berTreeInst.sub.at(0)->sub.at(2)->type = typeMap["GETRESPONSE"].ber;
    si.berTreeInst.sub.at(0)->sub.at(2)->sub.push_back(new BerTree());
    si.berTreeInst.sub.at(0)->sub.at(2)->sub.at(0)->type = typeMap["INTEGER"].ber;
    for (auto &i : requestID) {
        si.berTreeInst.sub.at(0)->sub.at(2)->sub.at(0)->content.push_back(i);
    }
    si.berTreeInst.sub.at(0)->sub.at(2)->sub.push_back(new BerTree());
    si.berTreeInst.sub.at(0)->sub.at(2)->sub.at(1)->type = typeMap["INTEGER"].ber;
    si.berTreeInst.sub.at(0)->sub.at(2)->sub.push_back(new BerTree());
    si.berTreeInst.sub.at(0)->sub.at(2)->sub.at(2)->type = typeMap["INTEGER"].ber;

    si.berTreeInst.sub.at(0)->sub.at(2)->sub.push_back(new BerTree());
    si.berTreeInst.sub.at(0)->sub.at(2)->sub.at(3)->type = typeMap["SEQUENCE"].ber;

    si.assignBerTreeLength(si.berTreeInst);
}

void PDUResponse::makeWrongOidPDU(SNMPDeserializer &di, SNMPSerializer &si) {
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

void PDUResponse::makeWrongValuePDU(SNMPSerializer &si, Tree &tree) {
    si.berTreeInst.sub.at(0)->sub.at(2)->sub.at(1)->content.push_back(errorValue);
    si.berTreeInst.sub.at(0)->sub.at(2)->sub.at(2)->content.push_back(errorIndex);

    for (int i = 0; i < oidList.size(); ++i) {
        si.berTreeInst.sub.at(0)->sub.at(2)->sub.at(3)->sub.push_back(new BerTree());
        si.berTreeInst.sub.at(0)->sub.at(2)->sub.at(3)->sub.back()->type = typeMap["SEQUENCE"].ber;
        si.berTreeInst.sub.at(0)->sub.at(2)->sub.at(3)->sub.back()->sub.push_back(new BerTree());
        si.berTreeInst.sub.at(0)->sub.at(2)->sub.at(3)->sub.back()->sub.back()->type = typeMap["OBJECT IDENTIFIER"].ber;
        std::vector<long> fullOid;
        for (auto &n : tree.node.at(oidList.at(i)).oid) {
            fullOid.push_back(n);
        }
        for (auto &n : tree.node.at(oidList.at(i)).value.at(i).id) {
            fullOid.push_back(n);
        }
        si.berTreeInst.sub.at(0)->sub.at(2)->sub.at(3)->sub.back()->sub.back()->content = si.getOidBer(fullOid);
        si.berTreeInst.sub.at(0)->sub.at(2)->sub.at(3)->sub.back()->sub.push_back(new BerTree());
        si.berTreeInst.sub.at(0)->sub.at(2)->sub.at(3)->sub.back()->sub.back()->type = typeMap["NULL"].ber;
    }

    si.assignBerTreeLength(si.berTreeInst);
}

bool PDUResponse::checkOidExistence(SNMPDeserializer &di, Tree &tree) {
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

        std::vector<long> oid = di.getOidValue(varbind->sub.at(0)->content);
        std::list<long> valueID;
        int oidIndex = -1;
        int valueIndex = -1;

        for (auto &p : oid) {
            printf("oid %ld ", p);
        }
        while (oid.size() && (oidIndex < 0)) {
            oidIndex = tree.findNode(oid);
            printf("ind %d ", oidIndex);
            if (oidIndex < 0) {
                valueID.push_front(oid.back());
                printf("vl %ld ", valueID.front());
                oid.pop_back();
            }
        }
        printf("Varbind OID index %d\n", oidIndex);
        if (oidIndex < 0) {
            errorIndex = varbindCount;
            errorValue = ERROR_NOSUCHNAME;
            return false;
        }
        oidList.push_back(oidIndex);
        valueIndex = tree.node.at(oidIndex).findValue(valueID);
        printf("Varbind value index %d\n", valueIndex);
        if (valueIndex < 0) {
            errorIndex = varbindCount;
            errorValue = ERROR_NOSUCHNAME;
            return false;
        }
        valueList.push_back(valueIndex);
    }
    return true;
}

bool PDUResponse::checkOidExistenceNext(SNMPDeserializer &di, Tree &tree) {
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

        std::vector<long> oid = di.getOidValue(varbind->sub.at(0)->content);
        std::list<long> valueID;
        int oidIndex = -1;
        int valueIndex = -1;

        while (oid.size() && (oidIndex < 0)) {
            oidIndex = tree.findNode(oid);
            printf("ind %d ", oidIndex);
            if (oidIndex < 0) {
                valueID.push_front(oid.back());
                printf("vl %ld ", valueID.front());
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
        printf("OID LAST %ld VALUE LAST %ld\n", oidList.back(), valueList.back());
    }
    return true;
}

bool PDUResponse::checkValueCorectness(Tree &tree) {
    int varbindCount = 0;
    for (auto &p : oidList) {
        varbindCount++;
        if (tree.node.at(p).access == "not-accessible") {
            errorValue = ERROR_NOSUCHNAME;
            errorIndex = varbindCount;
            return false;
        }
        if ((requestType == typeMap["GETREQUEST"].ber) || (requestType == typeMap["GETNEXTREQUEST"].ber)) {
            if ((tree.node.at(p).access == "write-only") || (!permissionRead)) {
                errorValue = ERROR_NOSUCHNAME;
                errorIndex = varbindCount;
                return false;
            }
        }
        if (requestType == typeMap["SETREQUEST"].ber) {
            if ((tree.node.at(p).access == "read-only") || (!permissionWrite)) {
                errorValue = ERROR_NOSUCHNAME;
                errorIndex = varbindCount;
                return false;
            }
        }
    }
    return true;
}
