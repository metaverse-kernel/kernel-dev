#ifndef STRING_H
#define STRING_H 1

#include <types.h>

extern void memset(void *__dest, u8 __src, usize len);
extern void memcpy(void *__dest, void *__src, usize len);
extern usize strlen(void *__dest);

#endif
