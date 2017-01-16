#include <vector>
#include <string>
#include <map>
#include "defines.h"
#include "SNMPSerializer.h"

SNMPSerializer::SNMPSerializer() {
}

SNMPSerializer::~SNMPSerializer() {
    berTreeInst.deleteTree();
}

std::list<char> SNMPSerializer::getLengthBer(long value) {
    std::list<char> lenghtList;
    if (value < 0) {
        lenghtList.push_back(0);
        return lenghtList;
    }
    if (value < 128) {
        lenghtList.push_back(value);
        return lenghtList;
    }
    while (value) {
        lenghtList.push_front(value & ((1<<8)-1));
        value = value >> 8;
    }
    lenghtList.push_front(lenghtList.size() | (1<<7));
}

std::list<char> SNMPSerializer::getIntBer(long value) {
    std::list<char> intList;
    if (value >= 0) {
        long check = 0x7F;
        for (int i = 1; i <=8; ++i) {
            if (value <= check) {
                for (int j = 0; j < i; ++j) {
                    intList.push_front(value & 0xFF);
                    value = value >> 8;
                }
                return intList;
            }
            check = (check << 8) | 0xFF;
        }
    } else {
        long check = (-1);
        check = (check << 7);
        for (int i = 1; i <=8; ++i) {
            if (value >= check) {
                for (int j = 0; j < i; ++j) {
                    intList.push_front(value & 0xFF);
                    value = value >> 8;
                }
                return intList;
            }
            check = (check << 8);
        }
    }
    return intList;
}

std::list<char> SNMPSerializer::getOidBer(std::vector<long> &oid) {
    std::list<char> oidList;
    int count = 1;
    for (auto &i : oid) {
        if (count == 1) {
            oidList.push_back(i);
            count++;
        } else if (count == 2) {
            oidList.back() = oidList.back()*40 + i;
            count++;
        } else {
            std::list<char> oidSubList;
            long n = i;
            oidSubList.push_front(n & 0x7F);
            n = n >> 7;
            while (n > 0) {
                oidSubList.push_front((n & 0x7F) | 0x80);
                n = n >> 7;
            }
            for (auto &subi : oidSubList) {
                oidList.push_back(subi);
            }
        }
    }
    return oidList;
}

std::list<char> SNMPSerializer::getIpBer(std::vector<long> &ip) {
    std::list<char> oidList;
    for (auto &i : ip) {
        oidList.push_back(i);
    }
    return oidList;
}

std::list<char> SNMPSerializer::getStrBer(std::string &value) {
    std::list<char> oidList;
    for (auto &i : value) {
        oidList.push_back(i);
    }
    return oidList;
}

void SNMPSerializer::makeResponseSkel(std::string communityString) {
    berTreeInst.content.clear();
    berTreeInst.deleteTree();
    berTreeInst.sub.push_back(new BerTree());
    berTreeInst.sub.at(0)->type = typeMap["SEQUENCE"].ber;

    berTreeInst.sub.at(0)->sub.push_back(new BerTree());
    berTreeInst.sub.at(0)->sub.at(0)->type = typeMap["INTEGER"].ber;
    berTreeInst.sub.at(0)->sub.at(0)->content.push_back(0x00);

    berTreeInst.sub.at(0)->sub.push_back(new BerTree());
    berTreeInst.sub.at(0)->sub.at(1)->type = typeMap["OCTET STRING"].ber;
    for (auto &cm : communityString) {
        berTreeInst.sub.at(0)->sub.at(1)->content.push_back(cm);
    }
}

void SNMPSerializer::assignBerTreeLength(BerTree& bt) {
    if (!bt.sub.empty()) {
        bt.content.clear();
        for (auto &node : bt.sub) {
            assignBerTreeLength(*node);
            bt.content.push_back(node->type);
            for (auto &l : node->length) {
                bt.content.push_back(l);
            }
            for (auto &c : node->content) {
                bt.content.push_back(c);
            }
        }
    }
    bt.length = getLengthBer(bt.content.size());
}
