#ifndef MIBPARSER_H
#define MIBPARSER_H

#include <string>
#include "Tree.h"
#include "defines.h"

class MIBParser {
public:
    MIBParser();
    ~MIBParser();
    void getFile(std::string fileName);
    void removeComments(std::string &line);
};



#endif /* MIBPARSER_H */
