#ifndef SYSCALL_H
#define SYSCALL_H 1

#ifdef __x86_64__
#include <kernel/arch/x86_64/syscall.h>
#endif

/**
 * @name syscall_init
 * 
 * ```c
 * void syscall_init();
 * ```
 * 
 * 初始化系统调用。
 */
void syscall_init();

#endif
