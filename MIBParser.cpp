#include <stdio.h>
#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>
//#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/regex.hpp>

#include "MIBParser.h"
#include "Tree.h"
#include "defines.h"

MIBParser::MIBParser() {
    DEBUG("Constructor\n");
}

MIBParser::~MIBParser() {
    DEBUG("Deconstructor\n");
}

void MIBParser::getFile(std::string fileName) {
    DEBUG("Read file");
    std::ifstream myfile(fileName);
    std::string line;
    if(myfile.is_open()) {
        while ( std::getline (myfile,line) ) {
            removeComments(line);
            std::cout << line << '\n';
        }
        myfile.close();
    }

}

void MIBParser::removeComments(std::string &line) {
    std::vector<std::string> v;
    boost::split_regex(v, line, boost::regex( "--" ));
    line = v.at(0);
}
