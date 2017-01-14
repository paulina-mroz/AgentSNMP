#ifndef PDURESPONSE_H
#define PDURESPONSE_H

#include <string>
#include "defines.h"
#include "BerTree.h"
#include "SNMPDeserializer.h"
#include "SNMPSerializer.h"
#include "MIBToolkit.h"
#include "Tree.h"
#include "Type.h"
#include "Value.h"

class PDUResponse {
public:
    PDUResponse();
    ~PDUResponse();

    void initPermissions(std::string fileName);
    bool getPermissions(std::string communityString, unsigned long menagerIP);

    void makeResponsePDU(SNMPDeserializer &di, SNMPSerializer &si, Tree &tree);
    void makeSkelPDU(SNMPSerializer &si);
    void makeWrongOidPDU(SNMPDeserializer &di, SNMPSerializer &si);
    void makeErrorPDU(SNMPSerializer &si, Tree &tree);
    void makeWrongSetPDU(SNMPSerializer &si, Tree &tree);
    bool makeGetPDU(SNMPSerializer &si, Tree &tree);

    bool checkOidExistence(SNMPDeserializer &di, Tree &tree);
    bool checkOidExistenceNext(SNMPDeserializer &di, Tree &tree);
    bool checkValueCorectness(SNMPDeserializer &di, Tree &tree);

    bool checkSetSize(Type &type, Value &v);
    bool checkSetRange(Type &type, Value &v);

    void saveValuesToTree(Tree &tree);


    MIBToolkit toolkitInst;

    struct permissionStruct {
        std::string cs;
        bool read;
        bool write;
        unsigned long ip;
        unsigned long mask;
    };
    std::vector<permissionStruct> permissions;

    char requestType;
    std::list<char> requestID;
    bool permissionRead;
    bool permissionWrite;
    int errorIndex;
    int errorValue;
    std::vector<int> oidList;
    std::vector<int> valueList;
    std::vector<Value> valueValues;
    int maxCount;

};

#endif /* PDURESPONSE_H */
