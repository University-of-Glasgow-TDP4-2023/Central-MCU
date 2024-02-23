#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdint.h> // For uint8_t, uint16_t, etc.

#define DEBUG 1

#if DEBUG
// DEBUG_PRINT only works if DEBUG is enabled.
#define DEBUG_PRINT(...) do { printf(__VA_ARGS__); } while(0)
#else
#define DEBUG_PRINT(fmt, args...) // Don't print anything
#endif

#endif // GLOBAL_H
