#ifndef SNMPSERVER_H
#define SNMPSERVER_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <string>
#include <list>
#include "SNMPDeserializer.h"
#include "SNMPSerializer.h"
#include "BerTree.h"
#include "defines.h"

class SNMPServer
{
public:
    SNMPServer();
    ~SNMPServer();
    bool initConnection();
    void receiveMessage();
    void sendResponse();

    SNMPDeserializer deserializerInst;
    SNMPSerializer serializerInst;

    struct sockaddr_in serverAddress;
    struct sockaddr_in clientAddress;
    int socketDescriptor;
    unsigned char recvBuf[RECVBUF_SIZE];
    unsigned char sendBuf[SENDBUF_SIZE];
    int recvBufLength;
    int sendBufLength;
};

#endif /* SNMPSERVER_H */
