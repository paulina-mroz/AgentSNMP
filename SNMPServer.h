#ifndef SNMPSERVER_H
#define SNMPSERVER_H

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
    void flow();
    void initConnection();
    void receiveMessage();
    void analyzeRequest();
    void createResponse();
    void sendResponse();

    bool checkVersion(BerTree &bt);
    bool checkCommunityString(BerTree &bt);
    bool checkPDU(BerTree &bt);
    void analyzePDU();

    SNMPDeserializer deserializerInst;
    SNMPSerializer serializerInst;

    int error;
    std::string communityString;
    // bool isPublic;
    // bool isPrivate;
    bool correctRequest;
    bool permissionRO;
    bool permissionRW;
    std::list<char> requestID;

    struct sockaddr_in serverAddress;
    struct sockaddr_in clientAddress;
    int socketDescriptor;
    unsigned char recvBuf[RECVBUF_SIZE];
    unsigned char sendBuf[SENDBUF_SIZE];
    int recvBufLength;
    int sendBufLength;
};


#endif /* SNMPSERVER_H */
