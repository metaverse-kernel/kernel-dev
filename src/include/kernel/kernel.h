#ifndef KERNEL_H
#define KERNEL_H 1

#include <types.h>
#include <asm/sysctl.h>

#define KERNEL_TODO() \
    while (true)      \
    {                 \
    }

#define simple_lock_lock(lock) \
    {                          \
        while (lock == true)   \
            ;                  \
        (lock) = true;         \
    }
#define simple_lock_unlock(lock) (lock) = false;

extern void *kend; // 内核结束的标记

#endif
