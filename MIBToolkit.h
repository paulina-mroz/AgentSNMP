#ifndef MIBTOOLKIT_H
#define MIBTOOLKIT_H

#include <string>
#include "defines.h"
#include "BerTree.h"
#include "SNMPDeserializer.h"
#include "SNMPSerializer.h"
#include "Tree.h"

class MIBToolkit {
public:
    MIBToolkit();
    ~MIBToolkit();
    void getPermissions(std::string communityString);
    // void makeResponsePDU(BerTree &dbt, BerTree &sbt);
    void makeResponsePDU(SNMPDeserializer &di, SNMPSerializer &si, Tree &tree);
    void makeSkelPDU(SNMPSerializer &si);
    void makeWrongOidPDU(SNMPDeserializer &di, SNMPSerializer &si);
    bool checkOidExistence(SNMPDeserializer &di, Tree &tree);
    bool checkOidExistenceNext(SNMPDeserializer &di, Tree &tree);

    std::list<char> requestID;
    bool permissionRO;
    bool permissionRW;
    int errorIndex;
    int errorValue;
    std::vector<int> oidList;
    std::vector<int> valueList;

};




#endif /* MIBTOOLKIT_H */
