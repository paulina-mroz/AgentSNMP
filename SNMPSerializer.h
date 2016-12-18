#ifndef SNMPSERIALIZER_H
#define SNMPSERIALIZER_H

#include <vector>
#include <string>
#include <map>
#include "defines.h"
#include "BerTree.h"

class SNMPSerializer {
public:
    SNMPSerializer();
    ~SNMPSerializer();

    void makeSerialData();

    BerTree berTreeInst;
    unsigned char berChar[RECVBUF_SIZE];


};

#endif /* SNMPSERIALIZER_H */
