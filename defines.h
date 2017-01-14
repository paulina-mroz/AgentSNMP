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

#define PORT            161
#define RECVBUF_SIZE    65507
#define SENDBUF_SIZE    65507

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

#define STORAGE_NONE    0
#define STORAGE_INT     1
#define STORAGE_OID     2
#define STORAGE_IP      3
#define STORAGE_STR     4

#endif /* DEFINES_H */
