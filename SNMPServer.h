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
    void flow(); // NOT
    bool initConnection();
    void receiveMessage();
    void analyzeRequest(); // NOT
    void createResponse();
    void sendResponse();

    bool checkVersion(BerTree &bt); // NOT
    bool checkCommunityString(BerTree &bt); // NOT
    bool checkPDU(BerTree &bt); // NOT
    void analyzePDU(BerTree &bt);

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
