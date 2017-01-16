#ifndef AGENTCLASS_h
#define AGENTCLASS_h

#include "defines.h"
#include "MIBParser.h"
#include "SNMPServer.h"
#include "PDUResponse.h"
#include "SNMPDeserializer.h"
#include "SNMPSerializer.h"
#include "MIBToolkit.h"

class AgentClass {
public:
    AgentClass();
    ~AgentClass();
    void init();
    void flow();
    void readContent();
    void makeContent();

    MIBParser parserInst;
    SNMPServer serverInst;
    SNMPDeserializer deserializerInst;
    SNMPSerializer serializerInst;
    PDUResponse responseInst;
    // MIBToolkit toolkitInst;

    bool debugPrint;
};

#endif /* AGENTCLASS_h */
