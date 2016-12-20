#ifndef SNMPSERVER_H
#define SNMPSERVER_H

#include <string>
#include "SNMPDeserializer.h"
#include "SNMPSerializer.h"
#include "BerTree.h"
#include "defines.h"

class SNMPServer
{
public:
    SNMPServer();
    ~SNMPServer();
    void flow();
    void initConnection();
    void receiveMessage();
    bool analyzeRequest();
    void createResponse();
    void sendResponse();

    bool errorInRequest(BerTree &bt);
    bool checkCommunityString(BerTree &bt);
    void analyzePDU(BerTree &bt);

    SNMPDeserializer deserializerInst;
    SNMPSerializer serializerInst;

    int error;

    struct sockaddr_in serverAddress;
    struct sockaddr_in clientAddress;
    int socketDescriptor;
    unsigned char recvBuf[RECVBUF_SIZE];
    unsigned char sendBuf[SENDBUF_SIZE];
    int recvBufLength;
    int sendBufLength;
};


#endif /* SNMPSERVER_H */
