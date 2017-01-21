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
            printf("Agent SNMP\nNote: You may need sudo privileges to run program.\n");
            printf("Usage: agent-snmp [OPTION]\n\n");
            printf("Options:\n");
            printf("\t-h, --help\t\tdisplay this help message\n");
            printf("\t-d, --debug\t\trun program in debug mode: dump input/output packets in hexadecimal and display PDU structure\n");
            printf("\t-t, --print_tree\tdisplay MIB tree\n");
            printf("\t-i, --interactive\trun program in interactive mode (also turns on the debug mode)\n");
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
                printf("Error: unable to enter interactive mode.\n");
                exit(-1);
            }
        }
        if ((!strcmp(argv[1], "--debug")) || (!strcmp(argv[1], "-d"))) {
            commandCorrect = true;
            agentInst.debugPrint = true;
        }
    }

    if (!commandCorrect) {
        printf("Error: command unknown. Type '--help' or '-h' for help.\n");
        return 0;
    }

    agentInst.flow();
    return 0;
}

void printMenu(std::vector<std::string> commands) {
    if (commands.size() == 1) {
        if ((commands.at(0) == "help") || (commands.at(0) == "h") || (commands.at(0) == "?")) {
            printf("Agent SNMP\n*** interactive mode ***\n\n");
            printf("Commands:\n");
            printf("\th, help\t\t\t\t\tdisplay this help message\n");
            printf("\tt, print_tree\t\t\t\tdisplay MIB tree\n");
            printf("\tn [NAME], print_node_name [NAME]\tdisplay information about node with the name NAME\n");
            printf("\to [OID], print_node_oid [OID]\t\tdisplay information about node with the object identifier OID\n");
            printf("\texit, quit\t\t\t\texit the program\n");
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
