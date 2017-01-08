#ifndef AGENTCLASS_h
#define AGENTCLASS_h

#include "defines.h"
// #include "Tree.h"
#include "MIBParser.h"
#include "SNMPServer.h"
#include "MIBToolkit.h"
#include "SNMPDeserializer.h"
#include "SNMPSerializer.h"

class AgentClass {
public:
    AgentClass();
    ~AgentClass();
    void getMIB();
    void flow();
    void readContent();
    void makeContent();

    // Tree treeInst;
    MIBParser parserInst;
    SNMPServer serverInst;
    SNMPDeserializer deserializerInst;
    SNMPSerializer serializerInst;
    MIBToolkit toolkitInst;

};




#endif /* AGENTCLASS_h */
