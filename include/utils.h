#ifndef UTILS_H
#define UTILS_H 1

#include <types.h>

#define DISALIGNED __attribute__((packed))

#define into_bytes(addr) ((u8 *)(addr))
#define bytes_into(bytes, type) ((type *)(bytes))

void pointer_to_string(u64 addr, char * dest);

#endif
