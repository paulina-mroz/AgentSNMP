#ifndef SNMPDESERIALIZER_H
#define SNMPDESERIALIZER_H

#include <list>
#include "defines.h"
#include "externs.h"
#include "BerTree.h"

class SNMPDeserializer {
public:
	SNMPDeserializer();
	~SNMPDeserializer();
	// void getSerialData(unsigned char data[]);
    void makeBerTree();
    void addBerNode(BerTree &bt);
    void readContent(unsigned char data[], int length);
    long getLength(std::list<char> berLength);

    BerTree berTreeInst;
    // std::list<char> contentList;
	int error;
};


#endif /* SNMPDESERIALIZER_H */
