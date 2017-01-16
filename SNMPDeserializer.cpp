#include <stdio.h>
#include <cstring>

#include "SNMPDeserializer.h"
#include "defines.h"

SNMPDeserializer::SNMPDeserializer() {
}

SNMPDeserializer::~SNMPDeserializer() {
    berTreeInst.deleteTree();
}

bool SNMPDeserializer::checkRequest() {
    communityString.clear();
    requestID.clear();

    if (berTreeInst.sub.size() != 1) {
        return false;
    }
    if (berTreeInst.sub.at(0)->type != typeMap["SEQUENCE"].ber) {
        return false;
    }
    if (berTreeInst.sub.at(0)->sub.size() != 3) {
        return false;
    }
    if (!checkVersion(*berTreeInst.sub.at(0)->sub.at(0))) {
        return false;
    }
    if (!checkCommunityString(*berTreeInst.sub.at(0)->sub.at(1))) {
        return false;
    }
    if (!checkPDU(*berTreeInst.sub.at(0)->sub.at(2))) {
        return false;
    }

    printf("Request correct :)\n");
    return true;
}

bool SNMPDeserializer::checkVersion(BerTree &bt) {
    return (bt.type == typeMap["INTEGER"].ber) && (getIntValue(bt.content) == 0);
}

bool SNMPDeserializer::checkCommunityString(BerTree &bt) {
    if (bt.type != typeMap["OCTET STRING"].ber) {
        return false;
    }
    communityString = std::string(bt.content.begin(), bt.content.end());
    return true;
}

bool SNMPDeserializer::checkPDU(BerTree &bt) {
    if ((bt.type != typeMap["GETREQUEST"].ber) && (bt.type != typeMap["SETREQUEST"].ber) && (bt.type != typeMap["GETNEXTREQUEST"].ber)) {
        return false;
    }
    if (bt.sub.size() != 4) {
        return false;
    }
    if ((bt.sub.at(0)->type != typeMap["INTEGER"].ber) || (bt.sub.at(1)->type != typeMap["INTEGER"].ber)
        || (bt.sub.at(2)->type != typeMap["INTEGER"].ber) || (bt.sub.at(3)->type != typeMap["SEQUENCE"].ber)) {
        return false;
    }
    if (bt.sub.at(0)->content.size() != 4) {
        return false;
    }
    requestID = bt.sub.at(0)->content;
    if (bt.sub.at(1)->content.size() != 1) {
        return false;
    }
    if (bt.sub.at(1)->content.front() != 0x00) {
        return false;
    }
    if (bt.sub.at(2)->content.size() != 1) {
        return false;
    }
    if (bt.sub.at(2)->content.front() != 0x00) {
        return false;
    }
    if (bt.sub.at(3)->type != typeMap["SEQUENCE"].ber) {
        return false;
    }
    return true;
}

void SNMPDeserializer::makeBerTree() {
    addBerNode(berTreeInst);
}

void SNMPDeserializer::addBerNode(BerTree &bt) {
    std::list<char> cl = bt.content;
    while(!cl.empty()) {
        bt.sub.push_back(new BerTree());
        bt.sub.back()->type = cl.front();
        cl.pop_front();
        if (!cl.empty()) {
            bt.sub.back()->length.push_back(cl.front());
            cl.pop_front();
            long lengthLong = 0;
            if (bt.sub.back()->length.front() & 0x80) {
                int lengthOfLength = bt.sub.back()->length.front() & 0x7F;
                if ((lengthOfLength > 0) && (lengthOfLength <= cl.size())) {
                    for (int i = 0; i < lengthOfLength; ++i) {
                        bt.sub.back()->length.push_back(cl.front());
                        cl.pop_front();
                        lengthLong = 256*lengthLong + (unsigned char)bt.sub.back()->length.back();
                    }
                } else {
                    return;
                }
            } else {
                lengthLong = bt.sub.back()->length.front();
            }
            if (lengthLong <= cl.size()) {
                for (long l = 0; l < lengthLong; ++l) {
                    bt.sub.back()->content.push_back(cl.front());
                    cl.pop_front();
                }
            } else {
                return;
            }
            bool isSequence = bt.sub.back()->type == typeMap["SEQUENCE"].ber;
            bool isGetRequest = bt.sub.back()->type == typeMap["GETREQUEST"].ber;
            bool isSetRequest = bt.sub.back()->type == typeMap["SETREQUEST"].ber;
            bool isGetNextRequest = bt.sub.back()->type == typeMap["GETNEXTREQUEST"].ber;
            bool isGetResponse = bt.sub.back()->type == typeMap["GETRESPONSE"].ber;
            if (isSequence || isGetRequest || isSetRequest || isGetNextRequest || isGetResponse) {
                addBerNode(*bt.sub.back());
            }
        }
    }
}

long SNMPDeserializer::getIntValue(std::list<char> &berInt) {
    long result;
    bool isSigned = true;
    for (auto &p : berInt) {
        if (isSigned) {
            result = p;
            isSigned = false;
        } else {
            result = 256*result + (unsigned char)p;
        }
    }
    // printf("getInt %d\n", result);
    return result;
}

std::vector<long> SNMPDeserializer::getOidValue(std::list<char> &berOid) {
    std::vector<long> oid;
    bool first = true;
    long n = 0;
    for (auto &i : berOid) {
        if (first) {
            oid.push_back(i/40);
            oid.push_back(i%40);
            first = false;
        } else {
            if (i & 0x80) {
                n = 128*n + (i & 0x7F);
            } else {
                n = 128*n + i;
                oid.push_back(n);
                n = 0;
            }
        }
    }
    return oid;
}

std::vector<long> SNMPDeserializer::getIpValue(std::list<char> &berIp) {
    std::vector<long> ip;
    for (auto &i : berIp) {
        ip.push_back(i);
    }
    return ip;
}

std::string SNMPDeserializer::getStrValue(std::list<char> &berStr) {
    std::string str;
    for (auto &i : berStr) {
        str.push_back(i);
    }
    return str;
}
