CC=g++
CFLAGS=-c
LDFLAGS=-lboost_regex -lpthread
SOURCES=Type.cpp Value.cpp Tree.cpp BerTree.cpp \
		SNMPDeserializer.cpp SNMPSerializer.cpp SNMPServer.cpp \
		MIBParser.cpp PDUResponse.cpp MIBToolkit.cpp \
		AgentClass.cpp main.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=agent-snmp

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o
	rm $(EXECUTABLE)
