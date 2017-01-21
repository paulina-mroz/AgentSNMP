#include <stdio.h>
#include <cstring>
#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <chrono>
#include <ratio>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/regex.hpp>

#include "MIBToolkit.h"
#include "Value.h"
#include "defines.h"

MIBToolkit::MIBToolkit(){
}

MIBToolkit::~MIBToolkit(){
}

void MIBToolkit::initUpTime() {
    startTime = std::chrono::steady_clock::now();
}


void MIBToolkit::initTreeValues(Tree &tree) {
    for (int i = 0; i < tree.node.size(); ++i) {
        if ((!tree.node.at(i).syntax.empty()) && (tree.node.at(i).access != "not-accessible")) {
            std::vector<long> id = tree.node.at(i).oid;
            id.pop_back();
            int ind = tree.findNode(id);
            if (ind >= 0) {
                if (tree.node.at(ind).index.empty()) {
                    Value v;
                    v.id.push_back(0);
                    tree.node.at(i).value.push_back(v);
                }
            }
        }
    }
}


void MIBToolkit::setHardcodedValues(Tree &tree) {
    int index = -1;

    index = tree.findNode("sysObjectID");
    if (index > -1) {
        tree.node.at(index).access = "read-write"; //for testing
    }

    index = tree.findNode("ifNumber");
    if (index > -1) {
        tree.node.at(index).access = "read-write"; //for testing
    }

    index = tree.findNode("sysLocation");
    if (index > -1) {
        tree.node.at(index).access = "not-accessible"; //for testing
        tree.node.at(index).value.at(0).valueStr = "not-accessible for testing";
    }

    index = tree.findNode("sysServices");
    if (index > -1) {
        tree.node.at(index).access = "read-write"; //for testing
        tree.node.at(index).type.range.at(1) = 10; //for testing
    }

    index = tree.findNode("sysContact");
    if (index > -1) {
        tree.node.at(index).value.at(0).valueStr = "contact@example";
    }

}

void MIBToolkit::setHardcodedTable(Tree &tree) {
    int index = -1;

    index = tree.findNode("ifEntry");
    if (index > -1) {
        std::string line;
        line = " 100 \" descr 1 \" 11 \" 1000 \" 666 \" 3.5.7.9 \" 3 \" 1 \" 0 \" 55 \" 1 \" 0 \"  \"  \"  \"  \"  \"  \"  \"  \"  \" 3.655 \"  ";
        setTableValues(tree, index, line);
        line = "  99 \" descr_2 \" 10 \" 170  \" 600 \"         \" 1 \" 3 \" 0 \" 8  \" 1 \" 0 \"  \"  \"  \"  \"  \"  \"  \"  \"  \" 1.3.6 \"  ";
        setTableValues(tree, index, line);
        line = "  10 \" descr_2 \" 10 \" 170  \" 600 \"         \" 1 \" 3 \" 0 \" 8  \" 1 \" 0 \"  \"  \"  \"  \"  \"  \"  \"  \"  \" 1.3.6 \"  ";
        setTableValues(tree, index, line);
    }


}

void MIBToolkit::setTableValues(Tree &tree, int tableIndex, std::string &line) {
    bool indexCorrect = true;
    std::string lineValues = line;

    boost::trim(lineValues);
    std::vector<std::string> values;
    boost::split_regex(values, lineValues, boost::regex( "[\"]{1}" ));

    for(auto &v : values) {
        boost::trim(v);
    }

    int indexCount = 0;
    std::list<long> valueID;
    for (auto &indexNumber : tree.node.at(tableIndex).indexIndex) {
        if (indexNumber < values.size()) {
            int indexID = tree.findNode(tree.node.at(tableIndex).index.at(indexCount));
            if (indexID >= 0) {
                std::list<long> valueSubID = getIdFromString(tree.node.at(indexID).type.storage, values.at(indexNumber));
                for (auto &subid : valueSubID) {
                    valueID.push_back(subid);
                }
                int valueExistID = tree.node.at(indexID).findValue(valueID);
                if (valueExistID >= 0) {
                    indexCorrect = false;
                }
            } else {
                indexCorrect = false;
            }
        } else {
            indexCorrect = false;
        }
        indexCount++;
    }

    if (indexCorrect) {
        for (int i = 0; i < tree.node.at(tableIndex).child.size(); ++i) {
            Value newValue;
            newValue.id = valueID;

            std::vector<long> newChildId = tree.node.at(tableIndex).oid;
            newChildId.push_back(tree.node.at(tableIndex).child.at(i));
            int newChildIndex = tree.findNode(newChildId);
            if (newChildIndex >= 0) {
                if (i < values.size()) {
                    int newStorage = tree.node.at(newChildIndex).type.storage;
                    if (newStorage == STORAGE_INT) {
                        newValue.valueInt = getIntFromString(values.at(i));
                    } else if (newStorage == STORAGE_IP) {
                        newValue.valueOidIp = getIpFromString(values.at(i));
                    } else if (newStorage == STORAGE_OID) {
                        newValue.valueOidIp = getOidFromString(values.at(i));
                    } else if (newStorage == STORAGE_STR) {
                        newValue.valueStr = values.at(i);
                    }
                }
                tree.node.at(newChildIndex).value.push_back(newValue);
            }
        }
    }

}

std::list<long> MIBToolkit::getIdFromString(int storage, std::string &value) {
    std::list<long> id;
    std::vector<long> vid;

    if (storage == STORAGE_INT) {
        vid.push_back(getIntFromString(value));
    } else if (storage == STORAGE_IP) {
        vid = getIpFromString(value);
    } else if (storage == STORAGE_OID) {
        vid = getOidFromString(value);
    }

    for (auto &p : vid) {
        id.push_back(p);
    }
    return id;
}

long MIBToolkit::getIntFromString(std::string &value) {
    std::string rgxStr = "^([-]*[\\d]+)$";
    std::regex rgx(rgxStr);

    std::smatch match;
    for(std::sregex_iterator i = std::sregex_iterator(value.begin(), value.end(), rgx); i != std::sregex_iterator(); ++i ) {
        match = *i;

        if (!match.str(1).empty()) {
            return std::stol(match.str(1));
        }
    }
    return 0;
}

std::vector<long> MIBToolkit::getIpFromString(std::string &value) {
    std::vector<long> ret;

    std::string rgxStr = "^([\\d]+)\\.([\\d]+)\\.([\\d]+)\\.([\\d]+)$";
    std::regex rgx(rgxStr);

    std::smatch match;
    for(std::sregex_iterator i = std::sregex_iterator(value.begin(), value.end(), rgx); i != std::sregex_iterator(); ++i ) {
        match = *i;

        if ((!match.str(1).empty()) && (!match.str(2).empty()) && (!match.str(3).empty()) && (!match.str(4).empty())) {
            ret.push_back(std::stol(match.str(1)));
            ret.push_back(std::stol(match.str(2)));
            ret.push_back(std::stol(match.str(3)));
            ret.push_back(std::stol(match.str(4)));
            return ret;
        }
    }
    ret.push_back(0);
    return ret;
}

std::vector<long> MIBToolkit::getOidFromString(std::string &value) {
    std::vector<long> ret;
    std::string rgxStr = "^([\\d]+[\\.\\d]*[\\d]+)$";
    std::regex rgx(rgxStr);

    std::smatch match;
    for(std::sregex_iterator i = std::sregex_iterator(value.begin(), value.end(), rgx); i != std::sregex_iterator(); ++i ) {
        match = *i;

        if (!match.str(1).empty()) {
            std::string oidStr = match.str(1);
            std::vector<std::string> v;
            boost::split_regex(v, oidStr, boost::regex( "\\." ));
            for (auto &p : v) {
                ret.push_back(std::stoi(p));
            }
            return ret;
        }
    }
    ret.push_back(0);
    return ret;
}

std::string MIBToolkit::saveIntToString(long value) {
    std::string ret;
    ret = std::to_string(value);
    return ret;
}

std::string MIBToolkit::saveOidIpToString(std::vector<long> value) {
    std::string ret;
    bool first = true;
    for (auto &p : value) {
        if (!first) {
            ret += ".";
        }
        ret += std::to_string(p);
        first = false;
    }
    return ret;
}


void MIBToolkit::updateValuesFromFile(Tree &tree, int index, int val) {
    bool valueCorrect = false;
    std::string block;

    if (index < 0) {
        return;
    }

    if (tree.node.at(index).name == "atEntry") {
        std::ifstream myfile("data/table_atEntry.data");
        std::string line;
        if(myfile.is_open()) {
            while (std::getline(myfile,line)) {
                setTableValues(tree, index, line);
            }
        }
        return;
    }

    if (tree.node.at(index).name == "sysUpTime") {
        std::chrono::steady_clock::time_point nowTime = std::chrono::steady_clock::now();
        auto upTime = std::chrono::duration_cast<std::chrono::milliseconds>(nowTime - startTime);
        tree.node.at(index).value.at(val).valueInt = (long)(upTime.count() / 10);
        return;
    }


    if (tree.node.at(index).name == "sysDescr") {
        std::vector<std::string> files;
        files.push_back("/proc/sys/kernel/ostype");
        files.push_back("/proc/sys/kernel/hostname");
        files.push_back("/proc/sys/kernel/osrelease");
        files.push_back("/proc/sys/kernel/version");

        for (auto &f: files) {
            std::ifstream myfile(f);
            std::string line;
            std::string subblock;
            if(myfile.is_open()) {
                while (std::getline(myfile,line)) {
                    subblock += line;
                }
            }
            boost::trim(subblock);
            block += subblock;
            block += " ";
        }

        valueCorrect = true;
    }

    if (tree.node.at(index).name == "sysObjectID") {
        std::ifstream myfile("data/sysObjectID.data");
        std::string line;
        if(myfile.is_open()) {
            while (std::getline(myfile,line)) {
                block += line;
            }
        }
        boost::trim(block);
        valueCorrect = true;
    }

    if (tree.node.at(index).name == "sysName") {
        std::ifstream myfile("/proc/sys/kernel/hostname");
        std::string line;
        if(myfile.is_open()) {
            while (std::getline(myfile,line)) {
                block += line;
            }
        }
        boost::trim(block);
        valueCorrect = true;
    }



    if (tree.node.at(index).name == "sysContact") {
        std::ifstream myfile("data/sysContact.data");
        std::string line;
        if(myfile.is_open()) {
            while (std::getline(myfile,line)) {
                block += line;
            }
        }
        boost::trim(block);
        valueCorrect = true;
    }

    if (tree.node.at(index).name == "ifNumber") {
        std::ifstream myfile("data/ifNumber.data");
        std::string line;
        if(myfile.is_open()) {
            while (std::getline(myfile,line)) {
                block += line;
            }
        }
        boost::trim(block);
        valueCorrect = true;
    }

    if (valueCorrect) {
        if (tree.node.at(index).value.empty()) {
            Value v;
            v.id.push_back(0);
            tree.node.at(index).value.push_back(v);
            val = 0;
        }

        int storage = tree.node.at(index).type.storage;
        if (storage == STORAGE_INT) {
            tree.node.at(index).value.at(val).valueInt = getIntFromString(block);
        } else if (storage == STORAGE_IP) {
            tree.node.at(index).value.at(val).valueOidIp = getIpFromString(block);
        } else if (storage == STORAGE_OID) {
            tree.node.at(index).value.at(val).valueOidIp = getOidFromString(block);
        } else if (storage == STORAGE_STR) {
            tree.node.at(index).value.at(val).valueStr = block;
        }
        return;
    }
}

void MIBToolkit::saveValuesToFile(Tree &tree, int index, int val) {
    std::string block;
    int storage = tree.node.at(index).type.storage;
    if (storage == STORAGE_INT) {
        block = saveIntToString(tree.node.at(index).value.at(val).valueInt);
    } else if (storage == STORAGE_IP) {
        block = saveOidIpToString(tree.node.at(index).value.at(val).valueOidIp);
    } else if (storage == STORAGE_OID) {
        block = saveOidIpToString(tree.node.at(index).value.at(val).valueOidIp);
    } else if (storage == STORAGE_STR) {
        block = tree.node.at(index).value.at(val).valueStr;
    }

    if (tree.node.at(index).name == "sysContact") {
        std::ofstream myfile("data/sysContact.data");
        myfile << block << std::endl;
    }

    if (tree.node.at(index).name == "sysObjectID") {
        std::ofstream myfile("data/sysObjectID.data");
        myfile << block << std::endl;
    }

    if (tree.node.at(index).name == "sysName") {
        std::ofstream myfile("/etc/hostname");
        myfile << block << std::endl;
    }

    if (tree.node.at(index).name == "ifNumber") {
        std::ofstream myfile("data/ifNumber.data");
        myfile << block << std::endl;
    }
}
