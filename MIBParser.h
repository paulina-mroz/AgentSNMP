#ifndef MIBPARSER_H
#define MIBPARSER_H

#include <string>
#include <map>
#include "Tree.h"
#include "Type.h"
#include "defines.h"

// enum enumState {
//     IDLE, IMPORT, OBJECTID
// };

class MIBParser {
public:
    MIBParser();
    ~MIBParser();
    void parseFile(std::string fileName);
    void getFile(std::string fileName, std::string &content);
    void removeComments(std::string &line);
    void makeBlock(const std::string &block);
    void addNode(std::string &parent, std::string &name, int number);
    void handleImports(const std::string &block);
    void handleObjectID(const std::string &block);
    void handleObjectType(const std::string &block);
    void handleParentFromBraces(std::string &child, std::string &blockParent);
    void handleTypeImplicit(const std::string &block);

    void addPrimaryType(const std::string block, Type &type);
    void initPrimaryTypes();

    Tree tree;
    std::map<std::string, Type> typeMap;

    //enumState state;
    //std::string buffer;
};



#endif /* MIBPARSER_H */
