#include <stdio.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <pthread.h>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/regex.hpp>

#include "AgentClass.h"
#include "defines.h"
#include "externs.h"

void printMenu(std::vector<std::string> commands);

AgentClass agentInst;

void *interactivePrint(void*) {
    while(1) {
        std::string command;
        std::cout << ">> ";
        std::getline(std::cin, command);

        boost::trim(command);
        std::vector<std::string> commands;
        boost::split_regex(commands, command, boost::regex( "[\\s]+" ));
        printMenu(commands);
    }
    pthread_exit(NULL);
}

int main(int argc, char **argv) {
    if (argc == 2) {
        if ((!strcmp(argv[1], "--help")) || (!strcmp(argv[1], "-h"))) {
            printf("Agent SNMP\n\thelp\n");
            return 0;
        }
    }

    pthread_t interactiveThread;
    agentInst.init();

    bool commandCorrect = false;
    if (argc == 1) {
        commandCorrect = true;
    }
    if (argc == 2) {
        if ((!strcmp(argv[1], "--print_tree")) || (!strcmp(argv[1], "-t"))) {
            commandCorrect = true;
            agentInst.parserInst.tree.printTree();
        }
        if ((!strcmp(argv[1], "--interactive")) || (!strcmp(argv[1], "-i"))) {
            commandCorrect = true;
            agentInst.debugPrint = true;
            int rc = pthread_create(&interactiveThread, NULL, interactivePrint, NULL);
            if (rc) {
                printf("Error: unable to enter interactive mode :(\n");
                exit(-1);
            }
        }
        if ((!strcmp(argv[1], "--debug")) || (!strcmp(argv[1], "-d"))) {
            commandCorrect = true;
            agentInst.debugPrint = true;
        }
    }

    if (!commandCorrect) {
        printf("Error: command unknown. Type '--help' or '-h' for help\n");
        return 0;
    }

    agentInst.flow();
    return 0;
}

void printMenu(std::vector<std::string> commands) {
    if (commands.size() == 1) {
        if ((commands.at(0) == "help") || (commands.at(0) == "h")) {
            printf("Agent SNMP\n\thelp\n");
        }
        if ((commands.at(0) == "print_tree") || (commands.at(0) == "t")) {
            agentInst.parserInst.tree.printTree();
        }
        if ((commands.at(0) == "exit") || (commands.at(0) == "quit")) {
            exit(0);
        }
    } else if (commands.size() == 2) {
        if ((commands.at(0) == "print_node_name") || (commands.at(0) == "n")) {
            std::string name = commands.at(1);
            agentInst.printNode(name);
        }
        if ((commands.at(0) == "print_node_oid") || (commands.at(0) == "o")) {
            std::string name = commands.at(1);
            std::vector<std::string> v;
            std::vector<long> v_int;
            boost::split_regex(v, name, boost::regex( "\\." ));
            try {
                for (auto &p : v) {
                    v_int.push_back(std::stoi(p));
                }
                agentInst.printNode(v_int);
            } catch (...) {
            }
        }
    }
}
