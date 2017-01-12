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
}

SNMPServer::~SNMPServer(){
}

bool SNMPServer::initConnection() {
    socketDescriptor = socket(AF_INET, SOCK_DGRAM, 0);

    if (socketDescriptor < 0) {
        perror("Error while opening socket");
        return false;
    }

    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(PORT);

    int error = bind(socketDescriptor, (struct sockaddr*) &serverAddress, sizeof(serverAddress));
    if (error < 0) {
        perror("Error while binding");
        return false;
    }

    listen(socketDescriptor,5);
    return true;
}

void SNMPServer::receiveMessage(){
    int clilen = sizeof(clientAddress);
    recvBufLength = recvfrom(socketDescriptor, recvBuf, RECVBUF_SIZE, 0, (struct sockaddr*)&clientAddress, (socklen_t*)&(clilen));
}

void SNMPServer::sendResponse() {
    sendto(socketDescriptor,sendBuf,sendBufLength,0,(struct sockaddr *)&clientAddress,sizeof(struct sockaddr));
    fflush(stdout);
}
