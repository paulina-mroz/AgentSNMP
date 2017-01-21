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
    valueValues.clear();

    requestID = di.berTreeInst.sub.at(0)->sub.at(2)->sub.at(0)->content;

    makeSkelPDU(si);
    correct = (requestType == typeMap["GETNEXTREQUEST"].ber) ? checkOidExistenceNext(di, tree) : checkOidExistence(di, tree);
    if (!correct) {
            makeWrongOidPDU(di, si);
            return;
    }

    maxCount = oidList.size();
    correct = checkValueCorectness(di, tree);
    if (!correct) {
        makeErrorPDU(si, tree);
        return;
    }
    if (requestType == typeMap["SETREQUEST"].ber) {
        saveValuesToTree(tree);
        for (int i = 0; i < oidList.size(); ++i) {
            toolkitInst.saveValuesToFile(tree, oidList.at(i), valueList.at(i));
        }
    }

    for (int i = 0; i < oidList.size(); ++i) {
        toolkitInst.updateValuesFromFile(tree, oidList.at(i), valueList.at(i));
    }
    correct = makeGetPDU(si, tree);
    if (!correct) {
        makeErrorPDU(si, tree);
        return;
    }
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
    si.berTreeInst.sub.at(0)->sub.at(2)->sub.at(1)->content.clear();
    si.berTreeInst.sub.at(0)->sub.at(2)->sub.at(2)->content.clear();
    si.berTreeInst.sub.at(0)->sub.at(2)->sub.at(1)->content.push_back(errorValue);
    si.berTreeInst.sub.at(0)->sub.at(2)->sub.at(2)->content.push_back(errorIndex);

    si.berTreeInst.sub.at(0)->sub.at(2)->sub.at(3)->deleteTree();

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

void PDUResponse::makeErrorPDU(SNMPSerializer &si, Tree &tree) {
    si.berTreeInst.sub.at(0)->sub.at(2)->sub.at(1)->content.clear();
    si.berTreeInst.sub.at(0)->sub.at(2)->sub.at(2)->content.clear();
    si.berTreeInst.sub.at(0)->sub.at(2)->sub.at(1)->content.push_back(errorValue);
    si.berTreeInst.sub.at(0)->sub.at(2)->sub.at(2)->content.push_back(errorIndex);

    si.berTreeInst.sub.at(0)->sub.at(2)->sub.at(3)->deleteTree();

    for (int i = 0; i < maxCount; ++i) {
        si.berTreeInst.sub.at(0)->sub.at(2)->sub.at(3)->sub.push_back(new BerTree());
        si.berTreeInst.sub.at(0)->sub.at(2)->sub.at(3)->sub.back()->type = typeMap["SEQUENCE"].ber;
        si.berTreeInst.sub.at(0)->sub.at(2)->sub.at(3)->sub.back()->sub.push_back(new BerTree());
        si.berTreeInst.sub.at(0)->sub.at(2)->sub.at(3)->sub.back()->sub.back()->type = typeMap["OBJECT IDENTIFIER"].ber;
        std::vector<long> fullOid;
        for (auto &n : tree.node.at(oidList.at(i)).oid) {
            fullOid.push_back(n);
        }
        for (auto &n : tree.node.at(oidList.at(i)).value.at(valueList.at(i)).id) {
            fullOid.push_back(n);
        }
        si.berTreeInst.sub.at(0)->sub.at(2)->sub.at(3)->sub.back()->sub.back()->content = si.getOidBer(fullOid);
        si.berTreeInst.sub.at(0)->sub.at(2)->sub.at(3)->sub.back()->sub.push_back(new BerTree());
        si.berTreeInst.sub.at(0)->sub.at(2)->sub.at(3)->sub.back()->sub.back()->type = typeMap["NULL"].ber;

        si.assignBerTreeLength(si.berTreeInst);
        if (si.berTreeInst.content.size() > SENDBUF_SIZE) {
            errorValue = ERROR_TOOBIG;
            errorIndex = i+1;
            maxCount = i;
            makeErrorPDU(si, tree);
        }
    }

}

void PDUResponse::makeWrongSetPDU(SNMPSerializer &si, Tree &tree) {
}

bool PDUResponse::makeGetPDU(SNMPSerializer &si, Tree &tree) {
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
        for (auto &n : tree.node.at(oidList.at(i)).value.at(valueList.at(i)).id) {
            fullOid.push_back(n);
        }
        si.berTreeInst.sub.at(0)->sub.at(2)->sub.at(3)->sub.back()->sub.back()->content = si.getOidBer(fullOid);
        si.berTreeInst.sub.at(0)->sub.at(2)->sub.at(3)->sub.back()->sub.push_back(new BerTree());

        std::list<char> valueBer;
        int storage = tree.node.at(oidList.at(i)).type.storage;

        if (storage == STORAGE_INT) {
            valueBer = si.getIntBer(tree.node.at(oidList.at(i)).value.at(valueList.at(i)).valueInt);
        } else if (storage == STORAGE_IP) {
            valueBer = si.getIpBer(tree.node.at(oidList.at(i)).value.at(valueList.at(i)).valueOidIp);
        } else if (storage == STORAGE_OID) {
            valueBer = si.getOidBer(tree.node.at(oidList.at(i)).value.at(valueList.at(i)).valueOidIp);
        } else if (storage == STORAGE_STR) {
            valueBer = si.getStrBer(tree.node.at(oidList.at(i)).value.at(valueList.at(i)).valueStr);
        }

        if ((valueBer.empty()) && (storage != STORAGE_STR)) {
            si.berTreeInst.sub.at(0)->sub.at(2)->sub.at(3)->sub.back()->sub.back()->type = typeMap["NULL"].ber;
        } else {
            si.berTreeInst.sub.at(0)->sub.at(2)->sub.at(3)->sub.back()->sub.back()->type = tree.node.at(oidList.at(i)).type.ber;
            si.berTreeInst.sub.at(0)->sub.at(2)->sub.at(3)->sub.back()->sub.back()->content = valueBer;
        }

        si.assignBerTreeLength(si.berTreeInst);
        if (si.berTreeInst.content.size() > SENDBUF_SIZE) {
            errorValue = ERROR_TOOBIG;
            errorIndex = i+1;
            maxCount = i+1;
            return false;
        }
    }

    return true;
}


bool PDUResponse::checkOidExistence(SNMPDeserializer &di, Tree &tree) {
    int varbindCount = 0;
    for (auto &varbind : di.berTreeInst.sub.at(0)->sub.at(2)->sub.at(3)->sub) {
        varbindCount++;

        if (varbind->type != typeMap["SEQUENCE"].ber) {
            errorIndex = varbindCount;
            errorValue = ERROR_GENERR;
            return false;
        }

        if (varbind->sub.size() != 2) {
            errorIndex = varbindCount;
            errorValue = ERROR_GENERR;
            return false;
        }

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
            if (oidIndex < 0) {
                valueID.push_front(oid.back());
                oid.pop_back();
            }
        }
        if (oidIndex < 0) {
            errorIndex = varbindCount;
            errorValue = ERROR_NOSUCHNAME;
            return false;
        }

        valueIndex = tree.node.at(oidIndex).findValue(valueID);
        if (valueIndex < 0) {
            errorIndex = varbindCount;
            errorValue = ERROR_NOSUCHNAME;
            return false;
        }

        oidList.push_back(oidIndex);
        valueList.push_back(valueIndex);
    }
    return true;
}

bool PDUResponse::checkOidExistenceNext(SNMPDeserializer &di, Tree &tree) {
    int varbindCount = 0;
    for (auto &varbind : di.berTreeInst.sub.at(0)->sub.at(2)->sub.at(3)->sub) {
        varbindCount++;

        if (varbind->type != typeMap["SEQUENCE"].ber) {
            errorIndex = varbindCount;
            errorValue = ERROR_GENERR;
            return false;
        }
        if (varbind->sub.size() != 2) {
            errorIndex = varbindCount;
            errorValue = ERROR_GENERR;
            return false;
        }
        if (varbind->sub.at(0)->type != typeMap["OBJECT IDENTIFIER"].ber) {
            errorIndex = varbindCount;
            errorValue = ERROR_GENERR;
            return false;
        }

        std::vector<long> oid = di.getOidValue(varbind->sub.at(0)->content);
        std::list<long> valueID;
        int oidIndex = -1;
        int valueIndex = -1;
        long childNumber = -1;
        bool found = false;

        while (oid.size() && (oidIndex < 0)) {
            oidIndex = tree.findNode(oid);
            if (oidIndex < 0) {
                childNumber = oid.back();
                valueID.push_front(oid.back());
                oid.pop_back();
            }
        }
        if (oidIndex < 0) {
            if (valueID.front() > tree.root.back()) {
                errorIndex = varbindCount;
                errorValue = ERROR_NOSUCHNAME;
                return false;
            }
            oid = tree.root;
            oidIndex = tree.findNode(oid);
            childNumber = -1;
        } else {
            if ((!tree.node.at(oidIndex).value.empty()) && (!valueID.empty())) {
                valueIndex = tree.node.at(oidIndex).findValue(valueID);

                if ((valueIndex < 0) || (tree.node.at(oidIndex).value.size() < (valueIndex+2))) {
                    childNumber = oid.back();
                    oid.pop_back();
                    oidIndex = tree.findNode(oid);
                } else {
                    oidList.push_back(oidIndex);
                    valueList.push_back(valueIndex+1);
                    found = true;
                }
            }
        }

        if (!found) {
            while ((!found) && (oid.size()>0)) {
                if (tree.node.at(oidIndex).child.empty()) {
                    if (tree.node.at(oidIndex).value.empty()) {
                        childNumber = oid.back();
                        oid.pop_back();
                        oidIndex = tree.findNode(oid);
                    } else {
                        valueIndex = 0;
                        found = true;
                    }
                } else {
                    int nextChildNumber = tree.node.at(oidIndex).findNextChild(childNumber);
                    if (nextChildNumber < 0) {
                        childNumber = oid.back();
                        oid.pop_back();
                        oidIndex = tree.findNode(oid);
                    } else {
                        childNumber = -1;
                        oid.push_back(tree.node.at(oidIndex).child.at(nextChildNumber));
                        oidIndex = tree.findNode(oid);
                    }
                }
            }

            if (found) {
                oidList.push_back(oidIndex);
                valueList.push_back(valueIndex);
            } else {
                errorIndex = varbindCount;
                errorValue = ERROR_NOSUCHNAME;
                return false;
            }
        }
    }
    return true;
}

bool PDUResponse::checkValueCorectness(SNMPDeserializer &di, Tree &tree) {
    int i = 0;
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
            std::vector<long> parentid = tree.node.at(p).oid;
            parentid.pop_back();
            int parentind = tree.findNode(parentid);
            if (parentind >= 0) {
                if (!tree.node.at(parentind).index.empty()) {
                    errorValue = ERROR_READONLY;
                    errorIndex = varbindCount;
                    return false;
                }
            }

            if (di.berTreeInst.sub.at(0)->sub.at(2)->sub.at(3)->sub.at(i)->sub.at(1)->type != tree.node.at(p).type.ber) {
                errorValue = ERROR_BADVALUE;
                errorIndex = varbindCount;
                return false;
            }

            Value v;
            int storage = tree.node.at(p).type.storage;

            if (storage == STORAGE_INT) {
                v.valueInt = di.getIntValue(di.berTreeInst.sub.at(0)->sub.at(2)->sub.at(3)->sub.at(i)->sub.at(1)->content);
            } else if (storage == STORAGE_IP) {
                v.valueOidIp = di.getIpValue(di.berTreeInst.sub.at(0)->sub.at(2)->sub.at(3)->sub.at(i)->sub.at(1)->content);
            } else if (storage == STORAGE_OID) {
                v.valueOidIp = di.getOidValue(di.berTreeInst.sub.at(0)->sub.at(2)->sub.at(3)->sub.at(i)->sub.at(1)->content);
            } else if (storage == STORAGE_STR) {
                v.valueStr = di.getStrValue(di.berTreeInst.sub.at(0)->sub.at(2)->sub.at(3)->sub.at(i)->sub.at(1)->content);
            }

            bool correct = false;
            std::string prim = "";
            correct = checkSetSize(tree.node.at(p).type, v);
            prim = tree.node.at(p).type.primaryType;
            while ((!prim.empty()) && correct) {
                correct = checkSetSize(typeMap[prim], v);
                prim = typeMap[prim].primaryType;
            }
            if (!correct) {
                errorValue = ERROR_BADVALUE;
                errorIndex = varbindCount;
                return false;
            }

            correct = checkSetRange(tree.node.at(p).type, v);
            prim = tree.node.at(p).type.primaryType;
            while ((!prim.empty()) && correct) {
                correct = checkSetRange(typeMap[prim], v);
                prim = typeMap[prim].primaryType;
            }
            if (!correct) {
                errorValue = ERROR_BADVALUE;
                errorIndex = varbindCount;
                return false;
            }
            valueValues.push_back(v);

        }
        i++;
    }
    return true;
}

bool PDUResponse::checkSetSize(Type &type, Value &v) {
    if (type.size.empty()) return true;

    long size = 0;
    if (type.storage == STORAGE_INT) {
        size = 1;
    } else if ((type.storage == STORAGE_OID) || (type.storage == STORAGE_IP)) {
        size = v.valueOidIp.size();
    } else if (type.storage == STORAGE_STR) {
        size = v.valueStr.size();
    }

    if ((size >= type.size.at(0)) && (size <= type.size.at(1))) return true;

    return false;
}

bool PDUResponse::checkSetRange(Type &type, Value &v) {
    if ((type.range.empty()) && (type.enumInts.empty())) return true;

    long range = 0;
    if (type.storage == STORAGE_INT) {
        range = v.valueInt;
    } else {
        return true;
    }

    if (!type.range.empty()) {
        if ((range >= type.range.at(0)) && (range <= type.range.at(1))) return true;
    }
    if (!type.enumInts.empty()) {
        for (auto &ei : type.enumInts) {
            if (ei.n == range) {
                return true;
            }
        }
    }

    return false;
}

void PDUResponse::saveValuesToTree(Tree &tree) {
    int i = 0;
    for (auto &p : oidList) {
        tree.node.at(p).value.at(valueList.at(i)).valueInt = valueValues.at(i).valueInt;
        tree.node.at(p).value.at(valueList.at(i)).valueOidIp = valueValues.at(i).valueOidIp;
        tree.node.at(p).value.at(valueList.at(i)).valueStr = valueValues.at(i).valueStr;

        i++;
    }
}
