#ifndef SNMPSERIALIZER_H
#define SNMPSERIALIZER_H

#include <vector>
#include <string>
#include <map>
#include <list>
#include "defines.h"
#include "BerTree.h"

class SNMPSerializer {
public:
    SNMPSerializer();
    ~SNMPSerializer();

    std::list<char> getLengthBer(long value);
    std::list<char> getIntBer(long value);
    void makeSerialData();

    BerTree berTreeInst;
    unsigned char berChar[RECVBUF_SIZE];


};

#endif /* SNMPSERIALIZER_H */
