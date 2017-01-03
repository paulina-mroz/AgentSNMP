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
	// void getSerialData(unsigned char data[]);
    bool checkRequest();
    bool checkVersion(BerTree &bt);
    bool checkCommunityString(BerTree &bt);
    bool checkPDU(BerTree &bt);

    void makeBerTree();
    void addBerNode(BerTree &bt);
    void readContent(unsigned char data[], int length);  // NOT
    // long getLength(std::vector<char> berLength);
    long getIntValue(std::list<char> &berInt);
    std::vector<int> getOidValue(std::list<char> &berOid);

    BerTree berTreeInst;
    std::string communityString;
    std::list<char> requestID;


    // std::list<char> contentList;
	int error;
};


#endif /* SNMPDESERIALIZER_H */
