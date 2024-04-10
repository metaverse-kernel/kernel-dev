#ifndef X86_64_SYSCALL
#define X86_64_SYSCALL 1

#include <types.h>

extern void *system_calls_table[256];

/**
 * @brief 系统调用规范 
 * @addindex x86_64
 *
 * 系统调用使用的寄存器：
 * rax - 调用号
 * rbx - 系统调用程序保留
 * rcx - rip寄存器缓存
 * rdi - 系统调用程序保留
 * rdx - 参数1
 * r8 - 参数2
 * r9 - 参数3
 * r10 - 参数4
 * r11 - rflags寄存器缓存
 * r12 - 参数5
 * r13 - 参数6
 * r14 - 参数7
 * r15 - 参数8
 *
 * 返回值 - rax
 * 
 * 系统调用时，使用内核主堆栈。
 */

/**
 * @name set_kernel_stack_cache
 * @addindex 平台依赖函数 x86_64
 * 
 * ```c
 * void set_kernel_stack_cache(usize stack);
 * ```
 * 
 * 将堆栈缓存设为一个`stack`。
 */
extern void set_kernel_stack_cache(usize stack);

/**
 * @name return_from_systemcall
 * @addindex 平台依赖函数 x86_64
 * 
 * ```c
 * void return_from_systemcall();
 * ```
 * 
 * 执行`sysret`指令。
 */
extern void return_from_systemcall();

#endif
