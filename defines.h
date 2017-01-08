#ifndef DEFINES_H
#define DEFINES_H


#define SHOW_DEBUG 0
#if SHOW_DEBUG
#define DEBUG(...) printf("%s(%d):\t%s: ", __FILE__, __LINE__, __FUNCTION__); printf(__VA_ARGS__); printf("\n");
#define DEBUG1     printf
#else
#define DEBUG(...)
#define DEBUG1
#endif

// #define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
// #define BYTE_TO_BINARY(byte)  \
//   (byte & 0x80 ? '1' : '0'), \
//   (byte & 0x40 ? '1' : '0'), \
//   (byte & 0x20 ? '1' : '0'), \
//   (byte & 0x10 ? '1' : '0'), \
//   (byte & 0x08 ? '1' : '0'), \
//   (byte & 0x04 ? '1' : '0'), \
//   (byte & 0x02 ? '1' : '0'), \
//   (byte & 0x01 ? '1' : '0')


#define PORT            161
#define RECVBUF_SIZE    1024
#define SENDBUF_SIZE    1024

#define ERROR_NOERROR       0
#define ERROR_TOOBIG        1
#define ERROR_NOSUCHNAME    2
#define ERROR_BADVALUE      3
#define ERROR_READONLY      4
#define ERROR_GENERR        5
// noError(0)
// There was no problem performing the request.
//
// tooBig(1)
// The response to your request was too big to fit into one response.
//
// noSuchName(2)
// An agent was asked to get or set an OID that it can't find; i.e., the OID doesn't exist.
//
// badValue(3)
// A read-write or write-only object was set to an inconsistent value.
//
// readOnly(4)
// This error is generally not used. The noSuchName error is equivalent to this one.
//
// genErr(5)
// This is a catch-all error. If an error occurs for which none of the previous messages is appropriate, a genError is issued.


#endif /* DEFINES_H */


//obrazki
//translate 3D
