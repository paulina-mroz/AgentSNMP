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
}

std::list<char> SNMPSerializer::getLengthBer(long value) {
    printf("\nGET LENGTH BER ");
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
        lenghtList.push_back(value & ((1<<8)-1));
        printf("%02X ", (unsigned char)lenghtList.back());
        value = value >> 8;
    }
    lenghtList.push_front(lenghtList.size() | (1<<7));
    printf("L: %02X ", (unsigned char)lenghtList.front());
}

std::list<char> SNMPSerializer::getIntBer(long value) {


}

void SNMPSerializer::makeSerialData() {

}
