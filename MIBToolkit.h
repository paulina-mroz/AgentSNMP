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

    bool permissionRO;
    bool permissionRW;
};




#endif /* MIBTOOLKIT_H */
