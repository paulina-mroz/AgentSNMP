#include <stdio.h>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <list>
#include <iostream>

#include "SNMPServer.h"
#include "defines.h"

SNMPServer::SNMPServer(){
    DEBUG("Constructor\n");
    error = 0;
}

SNMPServer::~SNMPServer(){
    DEBUG("Deconstructor\n");
}

void SNMPServer::flow() {
    initConnection();

    std::list<char> testList = serializerInst.getLengthBer(0);
    testList = serializerInst.getLengthBer(127);
    testList = serializerInst.getLengthBer(128);
    testList = serializerInst.getLengthBer(-1);
    testList = serializerInst.getLengthBer(500);

    testList = serializerInst.getIntBer(-128);
    testList = serializerInst.getIntBer(-129);
    testList = serializerInst.getIntBer(-1);
    testList = serializerInst.getIntBer(0);
    testList = serializerInst.getIntBer(127);
    testList = serializerInst.getIntBer(128);
    testList = serializerInst.getIntBer(256);

    while (1) {
        receiveMessage();
        if (!analyzeRequest()) {
            createResponse();
            sendResponse();
        }

    }
}

void SNMPServer::initConnection(){
    socketDescriptor = socket(AF_INET, SOCK_DGRAM, 0);

    if (socketDescriptor < 0){
        perror("Error while opening socket");
        error = -1;
    } else {
        DEBUG("Socket opened\n");
        memset(&serverAddress, 0, sizeof(serverAddress));
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_addr.s_addr = INADDR_ANY;
        serverAddress.sin_port = htons(PORT);

        error = bind(socketDescriptor, (struct sockaddr*) &serverAddress, sizeof(serverAddress));
        if (error < 0){
            perror("Error while binding");
        } else {
            DEBUG("Bind ok\n");
            listen(socketDescriptor,5);
        }
    }
}

void SNMPServer::receiveMessage(){
    int clilen = sizeof(clientAddress);
    // while(1) {
        printf("Waiting\n");
        recvBufLength = recvfrom(socketDescriptor, recvBuf, 1024, 0, (struct sockaddr*)&clientAddress, (socklen_t*)&(clilen));
        // printf("bufLength: %d\n", recvBufLength);
        // for (int i = 0; i < recvBufLength; ++i) {
        //     printf("0x%02X ",recvBuf[i]);
        // }
        // printf("\n");
    // }

    // clilen = sizeof(cli_addr);

    // while(1){
    //     printf("\nWaiting\n");
    //     int buf_l;
    //     buf_l = recvfrom(sockfd,buf_read,1024,0,(struct sockaddr *)&cli_addr, (socklen_t*)&clilen);
    //     printf("%d", buf_l);
    //     for (int i = 0; i < buf_l; ++i)
    //     {
    //         printf("%d ", buf_read[i]);
    //     }
    //     printf("\n");
    //     char send_data[] = "Response";
    //     sendto(sockfd,send_data,strlen(send_data),0,(struct sockaddr *)&cli_addr,sizeof(struct sockaddr));
    //     fflush(stdout);

}

void SNMPServer::sendResponse() {
    recvBuf[13] = 0xA2;
    sendBufLength = recvBufLength;
    for (int i = 0; i < sendBufLength; ++i) {
        sendBuf[i] = recvBuf[i];
        if (i % 16 == 0) {
            printf("\n%04d: ", i);
        }
        printf("%02X ",sendBuf[i]);
    }
    printf("\n");
    sendto(socketDescriptor,sendBuf,sendBufLength,0,(struct sockaddr *)&clientAddress,sizeof(struct sockaddr));
    fflush(stdout);
}

bool SNMPServer::analyzeRequest() {
    DEBUG("Analyze");
    deserializerInst.readContent(recvBuf, recvBufLength);
    // deserializerInst.berTreeInst.sub.clear();
    deserializerInst.berTreeInst.delete_tree();
    // delete deserializerInst.berTreeInst.sub.at(0);
    deserializerInst.makeBerTree();

    if (deserializerInst.berTreeInst.sub.size() == 1) {
        if (deserializerInst.berTreeInst.sub.at(0)->type == typeMap["SEQUENCE"].ber) {
            printf("SEQ OK size %d\n", deserializerInst.berTreeInst.sub.at(0)->sub.size());
            // deserializerInst.berTreeInst.print_tree(0);
            if (deserializerInst.berTreeInst.sub.at(0)->sub.size() == 3) {
                if ((checkVersion(*deserializerInst.berTreeInst.sub.at(0)->sub.at(0))) && (checkCommunityString(*deserializerInst.berTreeInst.sub.at(0)->sub.at(1)))) {
                    // analyzePDU(deserializerInst.berTreeInst.sub.at(0).sub.at(0));
                    return 0;
                } else {
                    // ERROR
                }
            } else {
                // ERROR
            }
        } else {
            // ERROR
        }
    } else {
        // ERROR
    }

    // deserializerInst.berTreeInst.sub.push_back(BerTree(0));
    // DEBUG("Clear\n");
    // deserializerInst.berTreeInst.sub.clear();

    // printf("HEX: %X", -129);
}

void SNMPServer::createResponse() {
    // serializerInst.berTreeInst.sub.clear();
    serializerInst.berTreeInst.delete_tree();
    serializerInst.berTreeInst.sub.push_back(new BerTree());
    serializerInst.berTreeInst.sub.at(0)->type = typeMap["SEQUENCE"].ber;

    // Version
    serializerInst.berTreeInst.sub.at(0)->sub.push_back(new BerTree());
    serializerInst.berTreeInst.sub.at(0)->sub.at(0)->type = typeMap["INTEGER"].ber;
    serializerInst.berTreeInst.sub.at(0)->sub.at(0)->content.push_back(0x00);



    serializerInst.berTreeInst.print_tree(0);
}

bool SNMPServer::checkVersion(BerTree &bt) {
    return (bt.type == typeMap["INTEGER"].ber) && (deserializerInst.getIntValue(bt.content) == 0);
}

bool SNMPServer::checkCommunityString(BerTree &bt) {
    // printf("BBBBBB %02X %02X \n", bt.type, typeMap["OCTET STRING"].ber);
    if (bt.type != typeMap["OCTET STRING"].ber) return false;
    // communityString = bt.content;
    // std::string cs(bt.content.begin(), bt.content.end());
    communityString = std::string(bt.content.begin(), bt.content.end());

    // for (auto &p : communityString) {
    //     std::cout << " AAA: " << p;
    // }
    std::cout << " CCC " << communityString;
    isPublic = communityString == "public";
    isPrivate = communityString == "private";
    std::cout << "public " << isPublic << " private " << isPrivate;
    return true;
}

void SNMPServer::analyzePDU(BerTree &bt) {

}
