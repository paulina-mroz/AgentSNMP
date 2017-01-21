#include <stdio.h>
#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>
#include <regex>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/regex.hpp>

#include "MIBParser.h"
#include "Tree.h"
#include "defines.h"
#include "externs.h"

std::map<std::string, Type> typeMap;

MIBParser::MIBParser() {
    initPrimaryTypes();
}

MIBParser::~MIBParser() {
}

void MIBParser::initPrimaryTypes() {
    typeMap["SEQUENCE"] = Type();
    typeMap["SEQUENCE"].ber = 0x30;
    typeMap["GETREQUEST"] = Type();
    typeMap["GETREQUEST"].ber = 0xA0;
    typeMap["SETREQUEST"] = Type();
    typeMap["SETREQUEST"].ber = 0xA3;
    typeMap["GETNEXTREQUEST"] = Type();
    typeMap["GETNEXTREQUEST"].ber = 0xA1;
    typeMap["GETRESPONSE"] = Type();
    typeMap["GETRESPONSE"].ber = 0xA2;

    typeMap["INTEGER"] = Type();
    typeMap["INTEGER"].ber = 0x02;
    typeMap["INTEGER"].storage = STORAGE_INT;
    typeMap["OCTET STRING"] = Type();
    typeMap["OCTET STRING"].ber = 0x04;
    typeMap["OCTET STRING"].storage = STORAGE_STR;
    typeMap["OBJECT IDENTIFIER"] = Type();
    typeMap["OBJECT IDENTIFIER"].ber = 0x06;
    typeMap["OBJECT IDENTIFIER"].storage = STORAGE_OID;
    typeMap["NULL"] = Type();
    typeMap["NULL"].ber = 0x05;

    typeMap["DisplayString"] = Type();
    typeMap["DisplayString"].primaryType = "OCTET STRING";
    typeMap["DisplayString"].ber = typeMap[typeMap["DisplayString"].primaryType].ber;
    typeMap["DisplayString"].storage = typeMap[typeMap["DisplayString"].primaryType].storage;
    typeMap["PhysAddress"] = Type();
    typeMap["PhysAddress"].primaryType = "OCTET STRING";
    typeMap["PhysAddress"].ber = typeMap[typeMap["PhysAddress"].primaryType].ber;
    typeMap["PhysAddress"].storage = typeMap[typeMap["PhysAddress"].primaryType].storage;

    // for (auto &p : typeMap) {
    //     std::cout << "INFO " << p.first << std::endl;
    //     p.second.printInfo();
    // }
}

void MIBParser::getFile(std::string fileName, std::string &content) {
    std::ifstream myfile(fileName);
    std::string line;
    content.clear();
    if(myfile.is_open()) {
        while (std::getline(myfile,line)) {
            removeComments(line);
            content += line + "\n";
        }
        myfile.close();
    }
}

void MIBParser::removeComments(std::string &line) {
    std::vector<std::string> v;
    boost::split_regex(v, line, boost::regex( "--" ));
    line = v.at(0);
}

void MIBParser::addNode(std::string &parent, std::string &name, int number) {
    if (parent.empty()) return;
    if (tree.findNode(name) >= 0) return;

    int ind = tree.findNode(parent);
    std::vector<long> vid;
    vid.clear();
    vid.push_back(1);
    if (ind < 0) {
        tree.node.push_back(Node());
        tree.node.back().name = parent;
        tree.node.back().oid = vid;
        tree.root = vid;
    }
    tree.node.push_back(Node());
    tree.node.back().name = name;
    tree.node.back().oid = tree.node.at(tree.findNode(parent)).oid;
    tree.node.back().oid.push_back(number);

    tree.node.at(tree.findNode(parent)).child.push_back(number);
    tree.node.at(tree.findNode(parent)).childp[number] = tree.node.end();
    std::sort(tree.node.at(tree.findNode(parent)).child.begin(), tree.node.at(tree.findNode(parent)).child.end());
}

void MIBParser::parseFile(std::string fileName) {
    std::string content;
    getFile(fileName, content);
    handleImports(content);
    handleObjectID(content);
    handleObjectType(content);
    setIndexIndex();
}

void MIBParser::setIndexIndex() {
    for (auto &n : tree.node) {
        n.indexIndex.clear();
        if (!n.index.empty()) {
            for (auto &ind : n.index) {
                int nodeIndex = tree.findNode(ind);
                if (nodeIndex >= 0) {
                    long childNumber = tree.node.at(nodeIndex).oid.back();
                    n.indexIndex.push_back(n.findChild(childNumber));
                }
            }
        }
    }
}


void MIBParser::handleImports(const std::string &block) {
    std::regex rgx("IMPORTS[\\s]*[^;]*");
    std::smatch match;

    if (std::regex_search(block.begin(), block.end(), match, rgx)) {
        std::regex rgx2("FROM[\\s]+([\\w-_]+)");
        std::smatch match2;
        const std::string blockImport = match.str(0);

        for(std::sregex_iterator i = std::sregex_iterator(blockImport.begin(), blockImport.end(), rgx2); i != std::sregex_iterator(); ++i ) {
            match2 = *i;

            std::string name = match2.str(1);
            if (name != "RFC-1212") {
                std::string fileName = "mibs/" + name + ".txt";
                std::string contentImport;
                getFile(fileName, contentImport);
                handleObjectID(contentImport);
                handleTypeImplicit(contentImport);
            }

            // for (auto &p : typeMap) {
            //     std::cout << "INFO " << p.first << std::endl;
            //     p.second.printInfo();
            // }
        }
    }
}

void MIBParser::handleObjectID(const std::string &block) {
    std::regex rgx("[\\s]*([\\w-]+)[\\s]+OBJECT IDENTIFIER[\\s]+::=[\\s]+\\{(([^\\}])*)\\}");
    std::smatch match;

    for(std::sregex_iterator i = std::sregex_iterator(block.begin(), block.end(), rgx); i != std::sregex_iterator(); ++i ) {
        match = *i;

        std::string blockNodes = match.str(2);
        std::string name = match.str(1);
        handleParentFromBraces(name,blockNodes);
    }
}

void MIBParser::handleObjectType(const std::string &block) {
    std::string rgxStrOT = "[\\s]*([\\w-]+)[\\s]+OBJECT-TYPE[\\s]+";
    std::string rgxStrSyntax = "SYNTAX(?:[\\s]+([^\\n]+)[\\s]+|[\\s]+([\\w]+[\\s]+\\{[^\\}]+\\})[\\s]+)";
    std::string rgxStrAccess = "ACCESS[\\s]+([^\\n]+)[\\s]+STATUS[\\s]+([^\\n]+)[\\s]+DESCRIPTION[\\s]+\"([^\"]+)\"[\\s]+";
    std::string rgxStrIndex = "(?:INDEX[\\s]+\\{([^\\}]+)\\}[\\s]+)*";
    std::string rgxStrParent = "::=[\\s]+\\{([^\\}]*)\\}";
    std::string rgxStr = rgxStrOT + rgxStrSyntax + rgxStrAccess + rgxStrIndex + rgxStrParent;
    std::regex rgx(rgxStr);
    std::smatch match;

    for(std::sregex_iterator i = std::sregex_iterator(block.begin(), block.end(), rgx); i != std::sregex_iterator(); ++i ) {
        match = *i;

        std::string blockNodes = match.str(8);
        std::string name = match.str(1);

        handleParentFromBraces(name,blockNodes);

        if (!match.str(2).empty()) {
            tree.node.at(tree.findNode(name)).syntax = match.str(2);
        } else {
            tree.node.at(tree.findNode(name)).syntax = match.str(3);
        }
        tree.node.at(tree.findNode(name)).access = match.str(4);
        tree.node.at(tree.findNode(name)).status = match.str(5);
        tree.node.at(tree.findNode(name)).description = match.str(6);

        if (!match.str(7).empty()) {
            std::string indicesBlock = match.str(7);
            std::vector<std::string> indices;
            boost::split_regex(indices, indicesBlock, boost::regex( "," ));
            for (auto &p : indices) {
                boost::trim(p);
                tree.node.at(tree.findNode(name)).index.push_back(p);
            }
        }

        addType(tree.node.at(tree.findNode(name)).syntax, tree.node.at(tree.findNode(name)).type);
        tree.node.at(tree.findNode(name)).type.ber = typeMap[tree.node.at(tree.findNode(name)).type.primaryType].ber;
        tree.node.at(tree.findNode(name)).type.storage = typeMap[tree.node.at(tree.findNode(name)).type.primaryType].storage;
        // std::` << "INFO " << name << std::endl;
        // tree.node.at(tree.findNode(name)).type.printInfo();
    }

}

void MIBParser::handleParentFromBraces(std::string &child, std::string &blockParent) {
    boost::trim(blockParent);

    std::string nameChild;
    std::string nameParent;
    int number;
    std::vector<std::string> nameParents;

    boost::split_regex(nameParents, blockParent, boost::regex( "[\\s]+" ));

    if (nameParents.size() >= 2) {
        std::vector<std::string>::iterator it;
        for (it=nameParents.begin(); it<nameParents.end()-1; ++it) {
            const std::string blockIt = *it;
            std::regex rgxIt("([\\w-]+)\\(*([\\d]*)\\)*");
            std::smatch matchIt;

            if (std::regex_search(blockIt.begin(), blockIt.end(), matchIt, rgxIt)) {

                nameChild = matchIt.str(1);
                if (matchIt.str(2).empty()) {
                    number = 1;
                } else {
                    number = std::stoi(matchIt.str(2));
                }
                if (!nameParent.empty()) {
                    addNode(nameParent, nameChild, number);
                }
                nameParent = nameChild;
            }
        }
        number = std::stoi(*it);
        nameChild = child;
        addNode(nameParent, nameChild, number);
    }
}

void MIBParser::handleTypeImplicit(const std::string &block) {
    std::regex rgx("[\\s]*([\\w-]+)[\\s]+::=[\\s]+\\[APPLICATION[\\s]+([\\d]+)\\][\\s]+IMPLICIT[\\s]+([^\\n]+)");
    std::smatch match;

    for(std::sregex_iterator i = std::sregex_iterator(block.begin(), block.end(), rgx); i != std::sregex_iterator(); ++i ) {
        match = *i;

        std::string name = match.str(1);
        int implicitNumber = std::stoi(match.str(2));
        const std::string blockType = match.str(3);

        addType(blockType, typeMap[name]);
        typeMap[name].storage = typeMap[typeMap[name].primaryType].storage;
        typeMap[name].ber = (1 << 6) | implicitNumber;
    }
    typeMap["IpAddress"].storage = STORAGE_IP;
    typeMap["NetworkAddress"] = Type();
    typeMap["NetworkAddress"].primaryType = "IpAddress";
    typeMap["NetworkAddress"].ber = typeMap[typeMap["NetworkAddress"].primaryType].ber;
    typeMap["NetworkAddress"].storage = typeMap[typeMap["NetworkAddress"].primaryType].storage;
}


void MIBParser::addType(const std::string block, Type &type) {
    std::string rgxStrPartPrim = "[ ]*([\\w]+[ ]*[\\w]*)[ ]*";
    std::string rgxStrPrim = "^" + rgxStrPartPrim + "$";

    std::string rgxStrPartRange = "\\(([\\d]+)\\.\\.([\\d]+)\\)";
    std::string rgxStrRange = "^" + rgxStrPartPrim + rgxStrPartRange;

    std::string rgxStrPartSize = "\\(SIZE \\(([\\d]+)(?:[\\.]{2}([\\d]+))*\\)\\)";
    std::string rgxStrSize = "^" + rgxStrPartPrim + rgxStrPartSize;

    std::string rgxStrPartEnum = "\\{([^\\}]+)\\}";
    std::string rgxStrEnum = rgxStrPartPrim + rgxStrPartEnum;

    std::regex rgxPrim(rgxStrPrim);

    std::smatch match;
    for(std::sregex_iterator i = std::sregex_iterator(block.begin(), block.end(), rgxPrim); i != std::sregex_iterator(); ++i ) {
        match = *i;

        std::string primary = match.str(1);
        boost::trim(primary);
        type.primaryType = primary;
    }

    std::regex rgxRange(rgxStrRange);
    for(std::sregex_iterator i = std::sregex_iterator(block.begin(), block.end(), rgxRange); i != std::sregex_iterator(); ++i ) {
        match = *i;

        std::string primary = match.str(1);
        boost::trim(primary);
        type.primaryType = primary;
        type.range.push_back(std::stol(match.str(2)));
        type.range.push_back(std::stol(match.str(3)));
    }

    std::regex rgxSize(rgxStrSize);
    for(std::sregex_iterator i = std::sregex_iterator(block.begin(), block.end(), rgxSize); i != std::sregex_iterator(); ++i ) {
        match = *i;

        std::string primary = match.str(1);
        boost::trim(primary);
        type.primaryType = primary;
        type.size.push_back(std::stol(match.str(2)));
        if (match.str(3).empty()) {
            type.size.push_back(std::stol(match.str(2)));
        } else {
            type.size.push_back(std::stol(match.str(3)));
        }
    }

    std::regex rgxEnum(rgxStrEnum);
    for(std::sregex_iterator i = std::sregex_iterator(block.begin(), block.end(), rgxEnum); i != std::sregex_iterator(); ++i ) {
        match = *i;

        std::string primary = match.str(1);
        boost::trim(primary);
        type.primaryType = primary;

        std::string blockItems = match.str(2);
        std::vector<std::string> items;
        boost::split_regex(items, blockItems, boost::regex( "," ));

        for (std::vector<std::string>::iterator it=items.begin(); it<items.end(); ++it) {
            const std::string blockIt = *it;
            std::regex rgxIt("([\\w-]+)\\(([\\d]+)\\)");
            std::smatch matchIt;

            if (std::regex_search(blockIt.begin(), blockIt.end(), matchIt, rgxIt)) {

                Type::enumInt ei;
                type.enumInts.push_back(ei);
                type.enumInts.back().s = matchIt.str(1);
                type.enumInts.back().n = std::stoi(matchIt.str(2));
            }
        }
    }
}
