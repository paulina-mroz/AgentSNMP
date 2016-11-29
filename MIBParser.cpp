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

MIBParser::MIBParser() {
    DEBUG("Constructor");
    // state = IDLE;
    // buffer.clear();

    // std::vector<int> newID;
    // newID.push_back(1);
    // std::vector<int> children;
    // children.push_back(3);
    // root = newID;
}

MIBParser::~MIBParser() {
    DEBUG("Deconstructor");
}

void MIBParser::getFile(std::string fileName, std::string &content) {
    DEBUG("Read file");
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
    DEBUG("NODE %d",tree.findNode(name));
    if (tree.findNode(name) >= 0) return;

    int ind = tree.findNode(parent);
    std::vector<int> vid;
    vid.clear();
    vid.push_back(1);
    if (ind < 0) {

        tree.node.push_back(Node());
    //     tree.node.back().child = children;
        tree.node.back().name = parent;
        tree.node.back().oid = vid;
        tree.root = vid;
    }
    tree.node.push_back(Node());
    tree.node.back().name = name;
    tree.node.back().oid = tree.node.at(tree.findNode(parent)).oid;
    tree.node.back().oid.push_back(number);

    tree.node.at(tree.findNode(parent)).child.push_back(number);
    std::sort(tree.node.at(tree.findNode(parent)).child.begin(), tree.node.at(tree.findNode(parent)).child.end());
}

void MIBParser::parseFile(std::string fileName) {
    std::string content;
    getFile(fileName, content);
    handleImports(content);
    handleObjectID(content);
    handleObjectType(content);
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
            // for (unsigned i=0; i<match2.size(); ++i)
            //     std::cout << "match #" << i << ": " << match2[i] << std::endl;
            std::string name = match2.str(1);
            if (name != "RFC-1212") {
                std::string fileName = "mibs/" + name + ".txt";
                std::string contentImport;
                getFile(fileName, contentImport);
                //std::cout << contentImport;
                handleObjectID(contentImport);
            }
        }

    }

}

void MIBParser::handleObjectID(const std::string &block) {
    DEBUG("");
    std::regex rgx("[\\s]*([\\w-]+)[\\s]+OBJECT IDENTIFIER[\\s]+::=[\\s]+\\{(([^\\}])*)\\}");
    std::smatch match;

    for(std::sregex_iterator i = std::sregex_iterator(block.begin(), block.end(), rgx); i != std::sregex_iterator(); ++i ) {
        match = *i;
        // for (unsigned i=0; i<match.size(); ++i)
        //     std::cout << "match #" << i << ": " << match[i] << std::endl;
        std::string blockNodes = match.str(2);
        std::string name = match.str(1);

        handleParentFromBraces(name,blockNodes);
    }
}

void MIBParser::handleObjectType(const std::string &block) {
    //std::regex rgx("\\n([\\w-]+)[\\s]+OBJECT-TYPE([^:]+)::=[\\s]+\\{(([^\\}])*)\\}");
    std::string rgxStrOT = "[\\s]*([\\w-]+)[\\s]+OBJECT-TYPE[\\s]+";
    // std::string rgxStrSyntax = "SYNTAX[\\s]+([^\\n]+)[\\s]+";
    // std::string rgxStrSyntax2 = "SYNTAX[\\s]+([\\w]+[\\s]+\\{[^\\}]+\\})[\\s]+";
    std::string rgxStrSyntax = "SYNTAX(?:[\\s]+([^\\n]+)[\\s]+|[\\s]+([\\w]+[\\s]+\\{[^\\}]+\\})[\\s]+)";
    std::string rgxStrAccess = "ACCESS[\\s]+([^\\n]+)[\\s]+STATUS[\\s]+([^\\n]+)[\\s]+DESCRIPTION[\\s]+\"([^\"]+)\"[\\s]+";
    // std::string rgxStrIndex = "INDEX[\\s]+(\\{[^\\}]+\\})[\\s]+";
    std::string rgxStrIndex = "(?:INDEX[\\s]+(\\{[^\\}]+\\})[\\s]+)*";
    std::string rgxStrParent = "::=[\\s]+\\{([^\\}]*)\\}";
    std::string rgxStr = rgxStrOT + rgxStrSyntax + rgxStrAccess + rgxStrIndex + rgxStrParent;
    // std::string rgxStr = rgxStrOT + rgxStrSyntax + rgxStrAccess + rgxStrIndex2 + rgxStrParent;
    std::regex rgx(rgxStr);
    std::smatch match;

    for(std::sregex_iterator i = std::sregex_iterator(block.begin(), block.end(), rgx); i != std::sregex_iterator(); ++i ) {
        match = *i;
        //std::cout << m.str() << " at position " << m.position() << '\n';
        // for (unsigned i=0; i<match.size(); ++i)
            // std::cout << "********** match #" << i << "[" << match.str(i).empty() << "]: " << match[i] << std::endl;

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

    }

}

void MIBParser::handleParentFromBraces(std::string &child, std::string &blockParent) {
    boost::trim(blockParent);

    std::string nameChild;
    std::string nameParent;
    int number;
    std::vector<std::string> nameParents;

    boost::split_regex(nameParents, blockParent, boost::regex( "[\\s]+" ));
    // int aaa = 0;
    // for (std::vector<std::string>::iterator it=nameParents.begin(); it<nameParents.end(); ++it) {
    //     std::cout << *it << "===" << aaa++ << "---";
    // }

    if (nameParents.size() >= 2) {
        std::vector<std::string>::iterator it;

        for (it=nameParents.begin(); it<nameParents.end()-1; ++it) {
            // std::cout << "|||||" << *it << "||||||";
            const std::string blockIt = *it;
            std::regex rgxIt("([\\w-]+)\\(*([\\d]*)\\)*");
            std::smatch matchIt;

            if (std::regex_search(blockIt.begin(), blockIt.end(), matchIt, rgxIt)) {
                //  for (unsigned i=0; i<matchIt.size(); ++i)
                //      std::cout << "match2 #" << i << ": " << matchIt[i] << std::endl;

                nameChild = matchIt.str(1);
                if (matchIt.str(2).empty()) {
                    number = 1;
                } else {
                    number = std::stoi(matchIt.str(2));
                }

                if (!nameParent.empty()) {
                    addNode(nameParent, nameChild, number);
                    // std::cout << "Parent: " << nameParent << ", child: " << nameChild << ", nr: " << number << std::endl;
                }

                nameParent = nameChild;
            }
        }
        number = std::stoi(*it);
        nameChild = child;
        addNode(nameParent, nameChild, number);
    }
}
