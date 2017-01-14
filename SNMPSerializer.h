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
    std::list<char> getOidBer(std::vector<long> &oid);
    std::list<char> getIpBer(std::vector<long> &ip);
    std::list<char> getStrBer(std::string &value);

    void assignBerTreeLength(BerTree& bt);

    BerTree berTreeInst;
    unsigned char berChar[RECVBUF_SIZE];
};

#endif /* SNMPSERIALIZER_H */
