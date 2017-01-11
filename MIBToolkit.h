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

    void setHardcodedValues(Tree &tree);

};




#endif /* MIBTOOLKIT_H */
