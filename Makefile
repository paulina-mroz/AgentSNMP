CC=g++
CFLAGS =-c
LDFLAGS=-lboost_regex
SOURCES=Type.cpp Tree.cpp SNMPServer.cpp SNMPDeserializer.cpp MIBParser.cpp main.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=agent-snmp

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o
