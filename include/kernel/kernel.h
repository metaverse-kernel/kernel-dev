#ifndef KERNEL_H
#define KERNEL_H 1

#include <types.h>

#ifdef __x86_64__

#include <kernel/arch/x86_64/kernel.h>

#define ISA_NAME "x86_64"

#endif

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

extern void kmain_rust();

extern void *kend; // 内核结束的标记

#endif
