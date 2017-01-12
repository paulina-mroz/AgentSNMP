#ifndef SNMPDESERIALIZER_H
#define SNMPDESERIALIZER_H

#include <list>
#include <vector>
#include "defines.h"
#include "externs.h"
#include "BerTree.h"

class SNMPDeserializer {
public:
	SNMPDeserializer();
	~SNMPDeserializer();

    bool checkRequest();
    bool checkVersion(BerTree &bt);
    bool checkCommunityString(BerTree &bt);
    bool checkPDU(BerTree &bt);

    void makeBerTree();
    void addBerNode(BerTree &bt);

    long getIntValue(std::list<char> &berInt);
    std::vector<long> getOidValue(std::list<char> &berOid);

    BerTree berTreeInst;
    std::string communityString;
    std::list<char> requestID;
};

#endif /* SNMPDESERIALIZER_H */
