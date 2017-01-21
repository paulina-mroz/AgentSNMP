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

    void printNode(std::string const& name);
    void printNode(std::vector<long> const& name);
    void printInternal(int index);

    MIBParser parserInst;
    SNMPServer serverInst;
    SNMPDeserializer deserializerInst;
    SNMPSerializer serializerInst;
    PDUResponse responseInst;

    bool debugPrint;
};

#endif /* AGENTCLASS_h */
