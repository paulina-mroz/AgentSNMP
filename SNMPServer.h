#ifndef SNMPSERVER_H
#define SNMPSERVER_H

#include "defines.h"

class SNMPServer
{
public:
    SNMPServer();
    ~SNMPServer();
    void initConnection();
    void receiveMessage();
    
    int error;
        
    struct sockaddr_in serverAddress;
    struct sockaddr_in clientAddress;
    int socketDescriptor;
    unsigned char recvBuf[RECVBUF_SIZE];
};


#endif /* SNMPSERVER_H */