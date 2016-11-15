#ifndef DEFINES_H
#define DEFINES_H


#define SHOW_DEBUG 1
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


#endif /* DEFINES_H */