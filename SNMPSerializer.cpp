#include <vector>
#include <string>
#include <map>
#include "defines.h"
#include "SNMPSerializer.h"

SNMPSerializer::SNMPSerializer() {
    DEBUG("Constructor");
}

SNMPSerializer::~SNMPSerializer() {
    DEBUG("Deconstructor");
    berTreeInst.delete_tree();
}

std::list<char> SNMPSerializer::getLengthBer(long value) {
    printf("\nGET LENGTH BER (%ld) ", value);
    std::list<char> lenghtList;
    if (value < 0) {
        lenghtList.push_back(0);
        printf("%02X ", lenghtList.back());
        return lenghtList;
    }
    if (value < 128) {
        lenghtList.push_back(value);
        printf("%02X ", lenghtList.back());
        return lenghtList;
    }
    while (value) {
        lenghtList.push_front(value & ((1<<8)-1));
        printf("%02X ", (unsigned char)lenghtList.front());
        value = value >> 8;
    }
    lenghtList.push_front(lenghtList.size() | (1<<7));
    printf("L: %02X ", (unsigned char)lenghtList.front());
}

std::list<char> SNMPSerializer::getIntBer(long value) {
    printf("\nGET INT BER (%ld) ", value);
    std::list<char> intList;
    if (value >= 0) {
        // do {
        //     intList.push_back(value & ((1<<8)-1));
        //     printf("%02X ", (unsigned char)intList.back());
        //     value = value >> 8;
        // } while (value != -1);
        long check = 0x7F;
        for (int i = 1; i <=8; ++i) {
            // printf("CHECK %lX\n ", check);
            if (value <= check) {
                for (int j = 0; j < i; ++j) {
                    intList.push_front(value & 0xFF);
                    value = value >> 8;
                    printf("%02X ", (unsigned char)intList.front());
                }
                return intList;
            }
            check = (check << 8) | 0xFF;
        }

    } else {
        long check = (-1);
        check = (check << 7);
        for (int i = 1; i <=8; ++i) {
            // printf("CHECK %lX\n ", check);
            if (value >= check) {
                for (int j = 0; j < i; ++j) {
                    intList.push_front(value & 0xFF);
                    value = value >> 8;
                    printf("%02X ", (unsigned char)intList.front());
                }
                return intList;
            }
            check = (check << 8);
        }
    }

    return intList;
}

void SNMPSerializer::makeSerialData() {

}
