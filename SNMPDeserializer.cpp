#include <stdio.h>
#include <cstring>

#include "SNMPDeserializer.h"
#include "defines.h"

SNMPDeserializer::SNMPDeserializer() {
    DEBUG("Constructor\n");
    error = 0;
}

SNMPDeserializer::~SNMPDeserializer() {
    DEBUG("Deconstructor\n");
}

void SNMPDeserializer::getSerialData(unsigned char data[]) {
	DEBUG("Sequence 0x%02x\n",data[0]);
	if(data[0] != 0x30) {
		perror("Not a sequence");
		error = -1;
	} else {
		int sequenceLength = data[1];
		DEBUG("Length 0x%02x\n",data[1]);
	}
}
