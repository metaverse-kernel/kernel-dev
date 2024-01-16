#ifndef BITS_H
#define BITS_H 1

#include <types.h>

#define bit_set(byte, n) (byte) |= (1 << (n));
#define bit_reset(byte, n) (byte) &= ~(1 << (n));
#define bit_get(byte, n) (((byte) & (1 << (n))) >> (n))

#define bitmap_set(map, n) bit_set(((u8 *)(map))[(n) / 8], (n) % 8);
#define bitmap_reset(map, n) bit_reset(((u8 *)(map))[(n) / 8], (n) % 8);
#define bitmap_get(map, n) bit_get(map[(n) / 8], (n) % 8)

// 向后对齐
#define align_to(addr, align)                      \
    if ((usize)(addr) % (align) != 0)              \
    {                                              \
        usize __align_to_tmp_addr = (usize)(addr); \
        __align_to_tmp_addr /= (align);            \
        __align_to_tmp_addr++;                     \
        __align_to_tmp_addr *= (align);            \
        addr = __align_to_tmp_addr;                \
    }

#define is_aligned(addr, align) \
    (addr % align == 0)

#endif
