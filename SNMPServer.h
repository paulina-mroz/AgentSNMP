#ifndef SNMPSERVER_H
#define SNMPSERVER_H

#include "SNMPDeserializer.h"
#include "defines.h"

class SNMPServer
{
public:
    SNMPServer();
    ~SNMPServer();
    void initConnection();
    void receiveMessage();
    
    SNMPDeserializer deserial1;

    int error;
        
    struct sockaddr_in serverAddress;
    struct sockaddr_in clientAddress;
    int socketDescriptor;
    unsigned char recvBuf[RECVBUF_SIZE];
};


#endif /* SNMPSERVER_H */