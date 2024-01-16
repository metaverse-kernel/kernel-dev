#ifndef UTILS_H
#define UTILS_H 1

#include <types.h>

#define UTILS_BIT_GET(byte, bit) ((byte) & (1 << (bit)))
#define UTILS_BIT_SET(byte, bit) ((byte) |= (1 << (bit)));
#define UTILS_BIT_RESET(byte, bit) ((byte) &= ~(1 << (bit)));

#define UTILS_BITMAP_GET(map, bit) (((u8 *)(map))[bit / 8] & (1 << ((bit) % 8)))
#define UTILS_BITMAP_SET(map, bit) (((u8 *)(map))[bit / 8] |= (1 << ((bit) % 8)));
#define UTILS_BITMAP_RESET(map, bit) (((u8 *)(map))[bit / 8] &= ~(1 << ((bit) % 8)));

#define DISALIGNED __attribute__((packed))

#endif
