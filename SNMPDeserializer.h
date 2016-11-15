#ifndef SNMPDESERIALIZER_H
#define SNMPDESERIALIZER_H

#include "defines.h"

class SNMPDeserializer
{
public:
	SNMPDeserializer();
	~SNMPDeserializer();
	void getSerialData(unsigned char data[]);
	
	int error;
};


#endif /* SNMPDESERIALIZER_H */