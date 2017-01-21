#include <stdio.h>
#include <cstring>
#include <iostream>

#include "AgentClass.h"
#include "defines.h"

AgentClass::AgentClass(){
    debugPrint = false;
}

AgentClass::~AgentClass(){
}

void AgentClass::init() {
    std::string fileMIB = "mibs/RFC1213-MIB.txt";
    std::string filePermissions = "data/community_string.conf";

    parserInst.parseFile(fileMIB);
    responseInst.toolkitInst.initUpTime();
    responseInst.toolkitInst.initTreeValues(parserInst.tree);
    responseInst.toolkitInst.setHardcodedValues(parserInst.tree);
    responseInst.toolkitInst.setHardcodedTable(parserInst.tree);
    responseInst.initPermissions(filePermissions);

    for (int i = 0; i < parserInst.tree.node.size(); ++i) {
        responseInst.toolkitInst.updateValuesFromFile(parserInst.tree, i, 0);
    }
}

void AgentClass::flow() {
    if (!serverInst.initConnection()) {
        printf("Error while creating server. Exiting...\n");
        return;
    }

    while (true) {
        serverInst.receiveMessage();
        readContent();

        if (serverInst.recvBufLength > 0) {
            if (debugPrint) {
                printf("\n\nReceived bytes");
                for (int i = 0; i < serverInst.recvBufLength; ++i) {
                    if (i % 16 == 0) {
                        printf("\n%04d: ", i);
                    }
                    printf("%02X ",serverInst.recvBuf[i]);
                }
                printf("\n\nReceived PDU\n");
                deserializerInst.berTreeInst.printTree();
                printf("\n");
            }
        }

        if (!deserializerInst.checkRequest()) {
        } else {
            if (responseInst.getPermissions(deserializerInst.communityString, serverInst.clientAddress.sin_addr.s_addr)) {
                serializerInst.makeResponseSkel(deserializerInst.communityString);
                responseInst.makeResponsePDU(deserializerInst,serializerInst,parserInst.tree);
                makeContent();

                if (debugPrint) {
                    int varbindCount = 0;
                    printf("Objects:\n");
                    for (auto &p : responseInst.oidList) {
                        varbindCount++;
                        printf("(%d) Object Identifier: ", varbindCount);
                        for (auto &o : parserInst.tree.node.at(p).oid) {
                            printf("%ld.", o);
                        }
                        long val = responseInst.valueList.at(varbindCount-1);
                        for (auto &o : parserInst.tree.node.at(p).value.at(val).id) {
                            printf("%ld.", o);
                        }
                        printf("\n");
                    }

                    printf("\nSending PDU\n");
                    serializerInst.berTreeInst.printTree();
                    printf("\nSending bytes");
                    for (int i = 0; i < serverInst.sendBufLength; ++i) {
                        if (i % 16 == 0) {
                            printf("\n%04d: ", i);
                        }
                        printf("%02X ",serverInst.sendBuf[i]);
                    }
                    printf("\n");
                }

                serverInst.sendResponse();
            }
        }
    }
}

void AgentClass::readContent() {
    deserializerInst.berTreeInst.content.clear();
    for (int i = 0; i < serverInst.recvBufLength; ++i) {
        deserializerInst.berTreeInst.content.push_back(serverInst.recvBuf[i]);
    }
    deserializerInst.berTreeInst.deleteTree();
    deserializerInst.makeBerTree();
}

void AgentClass::makeContent() {
    int i = 0;
    for (auto &c : serializerInst.berTreeInst.content) {
        serverInst.sendBuf[i] = c;
        i++;
    }
    serverInst.sendBufLength = serializerInst.berTreeInst.content.size();
}


void AgentClass::printNode(std::string const& name) {
    std::cout << "\nINFO about: " << name << std::endl;
    int index = parserInst.tree.findNode(name);
    if (index < 0) {
        std::cout << "Requested node does not exist :(" << std::endl;
        return;
    }
    printInternal(index);
}

void AgentClass::printNode(std::vector<long> const& name) {
    std::cout << "\nINFO about: ";
    parserInst.tree.printVector(name);
    std::cout << std::endl;
    int index = parserInst.tree.findNode(name);
    if (index < 0) {
        std::cout << "Requested node does not exist :(" << std::endl;
        return;
    }
    printInternal(index);
}

void AgentClass::printInternal(int index) {
    responseInst.toolkitInst.updateValuesFromFile(parserInst.tree, index, 0);
    parserInst.tree.node.at(index).printInfo();
}
