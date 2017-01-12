#include <stdio.h>
#include <cstring>
#include <iostream>

#include "AgentClass.h"
#include "defines.h"

AgentClass::AgentClass(){
    debug_print = false;
}

AgentClass::~AgentClass(){
}

void AgentClass::init() {
    std::string fileMIB = "mibs/RFC1213-MIB.txt";
    std::string filePermissions = "data/community_string.conf";

    parserInst.parseFile(fileMIB);
    toolkitInst.setHardcodedValues(parserInst.tree);
    responseInst.initPermissions(filePermissions);
}

void AgentClass::flow() {
    if (!serverInst.initConnection()) {
        printf("Error while creating server. Exiting...\n");
        return;
    }

    while (true) {
        if (debug_print) printf("\nWaiting for request...\n");
        serverInst.receiveMessage();
        readContent();

        if (debug_print) {
            printf("\nReceived");
            for (int i = 0; i < serverInst.recvBufLength; ++i) {
                if (i % 16 == 0) {
                    printf("\n%04d: ", i);
                }
                printf("%02X ",serverInst.recvBuf[i]);
            }
            printf("\n");
            printf("\n");
            deserializerInst.berTreeInst.print_tree(0);
            printf("\n");
        }

        if (!deserializerInst.checkRequest()) {
            printf("Wrong request :(\n");
        } else {
            if (responseInst.getPermissions(deserializerInst.communityString, serverInst.clientAddress.sin_addr.s_addr)) {
                serializerInst.makeResponseSkel(deserializerInst.communityString);
                responseInst.makeResponsePDU(deserializerInst,serializerInst,parserInst.tree);
                makeContent();

                if (debug_print) {
                    printf("\n");
                    serializerInst.berTreeInst.print_tree(0);
                    printf("\n");
                    printf("\nSending");
                    for (int i = 0; i < serverInst.sendBufLength; ++i) {
                        if (i % 16 == 0) {
                            printf("\n%04d: ", i);
                        }
                        printf("%02X ",serverInst.sendBuf[i]);
                    }
                    printf("\n");
                }

                serverInst.sendResponse();
            } else {
                printf("Do not got permissions :(\n");
            }
        }
    }
}

void AgentClass::readContent() {
    deserializerInst.berTreeInst.content.clear();
    for (int i = 0; i < serverInst.recvBufLength; ++i) {
        deserializerInst.berTreeInst.content.push_back(serverInst.recvBuf[i]);
    }
    deserializerInst.berTreeInst.delete_tree();
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
