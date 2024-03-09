#include <utils.h>

void pointer_to_string(u64 addr, char *dest)
{
    for (u8 i = 0; i < 16; i++)
    {
        char c = addr & 0xf;
        dest[15 - i] = (c < 10) ? c + '0' : c - 10 + 'a';
        addr >>= 4;
    }
}
