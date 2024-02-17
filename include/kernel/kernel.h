#ifndef KERNEL_H
#define KERNEL_H 1

#include <types.h>

#ifdef __x86_64__

#include <kernel/arch/x86_64/kernel.h>

#define ISA_NAME "x86_64"

#endif

/**
 * @name KERNEL_TODO
 * 
 * 表示此处还没有实现代码。
 */
#define KERNEL_TODO() \
    while (true)      \
    {                 \
    }

/**
 * @name simple_lock
 * 
 * 简易同步锁。有如下两个方法：
 * 
 * ```c
 * #define simple_lock_lock(lock)
 * ```
 * 
 * 获取一个锁。
 * 
 * ```c
 * #define simple_lock_unlock(lock)
 * ```
 * 
 * 解开一个锁。
 */
#define simple_lock_lock(lock) \
    {                          \
        while (lock == true)   \
            ;                  \
        (lock) = true;         \
    }
#define simple_lock_unlock(lock) (lock) = false;

/**
 * @name kmain_rust
 * 
 * rust内核主程序。
 */
extern void kmain_rust();

/**
 * @name kend
 * 
 * 内核结束的标记。
 * 
 * 此变量不代表任何值，但是此变量的地址被安排到了内核镜像的末尾，用于标志内核结束的位置。
 */
extern void *kend;

#endif
