#ifndef SNMPSERIALIZER_H
#define SNMPSERIALIZER_H

#include <vector>
#include <string>
#include <map>
#include <list>
#include "defines.h"
#include "externs.h"
#include "BerTree.h"

class SNMPSerializer {
public:
    SNMPSerializer();
    ~SNMPSerializer();

    void makeResponseSkel(std::string communityString);

    std::list<char> getLengthBer(long value);
    std::list<char> getIntBer(long value);
    std::list<char> getOidBer(std::vector<int> &oid);

    void assignBerTreeLength(BerTree& bt);
    void makeSerialData();

    BerTree berTreeInst;
    unsigned char berChar[RECVBUF_SIZE];


};

#endif /* SNMPSERIALIZER_H */
