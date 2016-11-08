#include <stdio.h>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "SNMPServer.h"
#include "defines.h"

SNMPServer::SNMPServer(){
    DEBUG("Constructor\n");
    error = 0;
}

SNMPServer::~SNMPServer(){
    DEBUG("Deconstructor\n");
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
    while(1) {
        DEBUG("Waiting\n");
        int bufLength;
        bufLength = recvfrom(socketDescriptor, recvBuf, 1024, 0, (struct sockaddr*)&clientAddress, (socklen_t*)&(clilen));
        DEBUG("bufLength: %d\n", bufLength);
        for (int i = 0; i < bufLength; ++i) {
            DEBUG("0x%02x %c",recvBuf[i],recvBuf[i]);
        }
        printf("\n");
    }
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