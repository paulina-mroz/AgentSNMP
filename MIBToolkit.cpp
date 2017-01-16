#include <stdio.h>
#include <cstring>
#include <iostream>
#include <regex>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/regex.hpp>

#include "MIBToolkit.h"
#include "Value.h"
#include "defines.h"

MIBToolkit::MIBToolkit(){
}

MIBToolkit::~MIBToolkit(){
}

void MIBToolkit::setHardcodedValues(Tree &tree) {
    int index = -1;
    index = tree.findNode("sysDescr"); //RO
    if (index > -1) {
        Value v;
        v.id.push_back(0);
        v.valueStr = "DESCRIPTION TesT";
        tree.node.at(index).value.push_back(v);
    }

    index = tree.findNode("sysContact"); //RW
    if (index > -1) {
        Value v;
        v.id.push_back(0);
        v.valueStr = "contact@example";
        tree.node.at(index).value.push_back(v);
        Value v2;
        v2.id.push_back(2);
        v2.valueStr = "contact@example";
        tree.node.at(index).value.push_back(v2);
        tree.node.at(index).oid.back() = 4000;
    }

    index = tree.findNode("sysName");
    if (index > -1) {
        Value v;
        v.id.push_back(0);
        v.valueStr = "access changed for testing";
        tree.node.at(index).value.push_back(v);
        tree.node.at(index).access = "not-accessible";
    }

    index = tree.findNode("sysServices");
    if (index > -1) {
        Value v;
        v.id.push_back(0);
        v.valueInt = 5;
        tree.node.at(index).value.push_back(v);
        tree.node.at(index).access = "read-write";
        tree.node.at(index).oid.back() = 4001;
    }
}

void MIBToolkit::setHardcodedTable(Tree &tree) {
// │     │        ├─ 1.3.6.1.2.1.2.2.1.1. ifIndex
// │     │        ├─ 1.3.6.1.2.1.2.2.1.2. ifDescr
// │     │        ├─ 1.3.6.1.2.1.2.2.1.3. ifType
// │     │        ├─ 1.3.6.1.2.1.2.2.1.4. ifMtu
// │     │        ├─ 1.3.6.1.2.1.2.2.1.5. ifSpeed
// │     │        ├─ 1.3.6.1.2.1.2.2.1.6. ifPhysAddress
// │     │        ├─ 1.3.6.1.2.1.2.2.1.7. ifAdminStatus
// │     │        ├─ 1.3.6.1.2.1.2.2.1.8. ifOperStatus
// │     │        ├─ 1.3.6.1.2.1.2.2.1.9. ifLastChange
// │     │        ├─ 1.3.6.1.2.1.2.2.1.10. ifInOctets
// │     │        ├─ 1.3.6.1.2.1.2.2.1.11. ifInUcastPkts
// │     │        ├─ 1.3.6.1.2.1.2.2.1.12. ifInNUcastPkts
// │     │        ├─ 1.3.6.1.2.1.2.2.1.13. ifInDiscards
// │     │        ├─ 1.3.6.1.2.1.2.2.1.14. ifInErrors
// │     │        ├─ 1.3.6.1.2.1.2.2.1.15. ifInUnknownProtos
// │     │        ├─ 1.3.6.1.2.1.2.2.1.16. ifOutOctets
// │     │        ├─ 1.3.6.1.2.1.2.2.1.17. ifOutUcastPkts
// │     │        ├─ 1.3.6.1.2.1.2.2.1.18. ifOutNUcastPkts
// │     │        ├─ 1.3.6.1.2.1.2.2.1.19. ifOutDiscards
// │     │        ├─ 1.3.6.1.2.1.2.2.1.20. ifOutErrors
// │     │        ├─ 1.3.6.1.2.1.2.2.1.21. ifOutQLen
// │     │        └─ 1.3.6.1.2.1.2.2.1.22. ifSpecific

    int index = tree.findNode("ifEntry");
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
    std::cout << lineValues << std::endl;

    boost::trim(lineValues);
    std::cout << lineValues << std::endl;
    std::vector<std::string> values;
    boost::split_regex(values, lineValues, boost::regex( "[\"]+" ));

    for(auto &v : values) {
        boost::trim(v);
        std::cout << "val ---" << v << "---" << std::endl;
    }

    std::cout << "CHILDREN COUNT " << tree.node.at(tableIndex).child.size() << std::endl;

    std::list<long> valueID;
    for (auto &indexNumber : tree.node.at(tableIndex).indexIndex) {
        if (indexNumber < values.size()) {
            int indexID = tree.findNode(tree.node.at(tableIndex).index.at(indexNumber));
            std::cout << "INDEX ID " << indexID << std::endl;
            if (indexID >= 0) {
                std::list<long> valueSubID = getIdFromString(tree.node.at(indexID).type.storage, values.at(indexNumber));
                for (auto &subid : valueSubID) {
                    valueID.push_back(subid);
                }
                int valueExistID = tree.node.at(indexID).findValue(valueID);
                std::cout << "INDEX EXIST ID " << valueExistID << std::endl;
                if (valueExistID >= 0) {
                    indexCorrect = false;
                }
            } else {
                indexCorrect = false;
            }
        } else {
            indexCorrect = false;
        }
    }

    if (indexCorrect) {
        std::cout << "INDEX OK" << std::endl;



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
    std::cout << value << std::endl;
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
    std::cout << "ID from string: ";
    for (auto &p : id) {
        std::cout << p << " ";
    }
    std::cout << std::endl;
    return id;
}

long MIBToolkit::getIntFromString(std::string &value) {
    std::string rgxStr = "^([-]*[\\d]+)$";
    std::regex rgx(rgxStr);

    std::smatch match;
    for(std::sregex_iterator i = std::sregex_iterator(value.begin(), value.end(), rgx); i != std::sregex_iterator(); ++i ) {
        match = *i;
        // for (unsigned i=0; i<match.size(); ++i)
        //     std::cout << "INT match #" << i << ": ---" << match[i] << "---" << std::endl;
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
        // for (unsigned i=0; i<match.size(); ++i)
        //     std::cout << "IP match #" << i << ": ---" << match[i] << "---" << std::endl;
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
        // for (unsigned i=0; i<match.size(); ++i)
        //     std::cout << "OID match #" << i << ": ---" << match[i] << "---" << std::endl;
        if (!match.str(1).empty()) {
            std::string oidStr = match.str(1);
            std::vector<std::string> v;
            // std::vector<long> v_int;
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

void MIBToolkit::updateValuesFromFile(Tree &tree) {

}
