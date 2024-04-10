#ifndef UTILS_H
#define UTILS_H 1

#include <types.h>

#define DISALIGNED __attribute__((packed))

#define into_bytes(addr) ((u8 *)(addr))
#define bytes_into(bytes, type) ((type *)(bytes))

void pointer_to_string(u64 addr, char *dest);

typedef struct __va_args
{
    usize length;
    void *args[0];
} va_args;

#define va_args_gen(result, length) \
    void *__reserved__[length];     \
    va_args __va_args__;            \
    result = &__va_args__;

#define va_args_set(vaargs, index, val) \
    vaargs->args[index] = &val;

#endif
