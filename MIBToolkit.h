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

    void initTreeValues(Tree &tree);
    void setHardcodedValues(Tree &tree);
    void setHardcodedTable(Tree &tree);
    void setTableValues(Tree &tree, int tableIndex, std::string &line);

    std::list<long> getIdFromString(int storage, std::string &value);
    long getIntFromString(std::string &value);
    std::vector<long> getIpFromString(std::string &value);
    std::vector<long> getOidFromString(std::string &value);

    std::string saveIntToString(long value);
    std::string saveOidIpToString(std::vector<long> value);

    void updateValuesFromFile(Tree &tree, int index, int val);
    void saveValuesToFile(Tree &tree, int index, int val);

};

#endif /* MIBTOOLKIT_H */
