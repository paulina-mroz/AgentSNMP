#include <stdio.h>
#include <cstring>

#include "SNMPDeserializer.h"
#include "defines.h"

SNMPDeserializer::SNMPDeserializer() {
    DEBUG("Constructor\n");
    error = 0;
}

SNMPDeserializer::~SNMPDeserializer() {
    DEBUG("Deconstructor\n");
    berTreeInst.delete_tree();
}

// void SNMPDeserializer::getSerialData(unsigned char data[]) {
// 	DEBUG("Sequence 0x%02x\n",data[0]);
// 	if(data[0] != 0x30) {
// 		perror("Not a sequence");
// 		error = -1;
// 	} else {
// 		int sequenceLength = data[1];
// 		DEBUG("Length 0x%02x\n",data[1]);
// 	}
// }

bool SNMPDeserializer::checkRequest() {
    berTreeInst.delete_tree();
    makeBerTree();

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

























void SNMPDeserializer::readContent(unsigned char data[], int length) {
    berTreeInst.content.clear();
    for (int i = 0; i < length; ++i) {
        if (i % 16 == 0) {
            printf("\n%04d: ", i);
        }
        printf("%02X ",data[i]);
        berTreeInst.content.push_back(data[i]);
    }
    printf("\n");
}

void SNMPDeserializer::makeBerTree() {
    // berTreeInst.sub.clear();
    // berTreeInst.sub.push_back(BerTree());
    // std::vector<BerTree>::iterator current = berTreeInst.sub.begin();
    // std::list<char> cl = berTreeInst.content;
    // printf("SEQUENCE %02X %02X\n",cl.front(),berTreeInst.sub.at(0).type);
    // // (*current).type = 0xCD;
    // while (!cl.empty()) {
    //     for (auto &p : cl) {
    //         printf("0x%02X ", (unsigned char)p);
    //     }
    //     printf("\n");
    //
    //     if (cl.front() == typeMap["SEQUENCE"].ber) {
    //         current->type = cl.front();
    //         printf("SEQUENCE %02X %02X\n",(unsigned char)cl.front(),berTreeInst.sub.at(0).type);
    //
    //     }
    //     cl.pop_front();
    // }
    addBerNode(berTreeInst);
    berTreeInst.print_tree(0);
    // printf("LENGTH TEST: %04X\n", (unsigned char)getLength(berTreeInst.sub.at(0).length));
}

void SNMPDeserializer::addBerNode(BerTree &bt) {
    std::list<char> cl = bt.content;

    while(!cl.empty()) {
        // for (auto &p : cl) {
        //     printf("0x%02X ", (unsigned char)p);
        // }
        // printf("\n");
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
                    // ERROR
                    return;
                }
            } else {
                lengthLong = bt.sub.back()->length.front();
            }
            // printf("LENGTH %d %X\n",lengthLong, lengthLong);
            if (lengthLong <= cl.size()) {
                for (long l = 0; l < lengthLong; ++l) {
                    bt.sub.back()->content.push_back(cl.front());
                    cl.pop_front();
                }
            } else {
                // ERROR
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

// long SNMPDeserializer::getLength(std::vector<char> berLength) {
//     long result = 0;
//     // if (berLength.front() & 0x80) {
//     //     int lengthOfLength = berLength.front() & 0x7F;
//     //         for (int i = 0; i < lengthOfLength; ++i) {
//     //             result = 256*result + (unsigned char)(berLength.at(i+1));
//     //         }
//     // } else {
//     //     result = berLength.front();
//     // }
//     return result;
// }


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
    printf("getInt %d\n", result);
    return result;
}

std::vector<int> SNMPDeserializer::getOidValue(std::list<char> &berOid) {
    std::vector<int> oid;
    bool first = true;
    for (auto &i : berOid) {
        if (first) {
            oid.push_back(i/40);
            oid.push_back(i%40);
            first = false;
        } else {
            oid.push_back(i);
        }
    }
    return oid;
}
