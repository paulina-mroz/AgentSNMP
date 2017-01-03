#include <stdio.h>
#include <cstring>
#include <iostream>

#include "AgentClass.h"
#include "defines.h"

AgentClass::AgentClass(){
    DEBUG("Constructor\n");
}

AgentClass::~AgentClass(){
    DEBUG("Deconstructor\n");
}

void AgentClass::getMIB() {
    std::string fileMIB = "mibs/RFC1213-MIB.txt";
    parserInst.parseFile(fileMIB);
}
void AgentClass::flow() {
    if (!serverInst.initConnection()) {
        printf("Error while creating server. Exiting...\n");
        return;
    }

    while (true) {
        printf("Waiting for request...\n");
        serverInst.receiveMessage();
        readContent();
        if (!deserializerInst.checkRequest()) {
            printf("Wrong request :(\n");
        } else {
            toolkitInst.getPermissions(deserializerInst.communityString);
            serializerInst.makeResponseSkel(deserializerInst.communityString);
            // toolkitInst.makeResponsePDU(*deserializerInst.berTreeInst.sub.at(0)->sub.at(2),*serializerInst.berTreeInst.sub.at(0)->sub.at(2));
            toolkitInst.makeResponsePDU(deserializerInst,serializerInst,parserInst.tree);
            serializerInst.berTreeInst.print_tree(0);
            printf("CONTENT: ");
            for (auto &c : serializerInst.berTreeInst.content) {
                printf("%02X ", (unsigned char)c);
            }
            printf("\n");
            serverInst.sendResponse();
        }
    }
}

void AgentClass::readContent() {
    deserializerInst.berTreeInst.content.clear();
    for (int i = 0; i < serverInst.recvBufLength; ++i) {
        if (i % 16 == 0) {
            printf("\n%04d: ", i);
        }
        printf("%02X ",serverInst.recvBuf[i]);
        deserializerInst.berTreeInst.content.push_back(serverInst.recvBuf[i]);
    }
    printf("\n");
}