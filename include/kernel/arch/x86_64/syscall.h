#ifndef X86_64_SYSCALL
#define X86_64_SYSCALL 1

#include <types.h>

extern void *system_calls_table[256];

// 系统调用使用的寄存器：
// rax - 调用号
// rbx - 系统调用程序保留
// rcx - rip寄存器缓存
// rdi - 系统调用程序保留
// rdx - 参数1
// r8 - 参数2
// r9 - 参数3
// r10 - 参数4
// r11 - rflags寄存器缓存
// r12 - 参数5
// r13 - 参数6
// r14 - 参数7
// r15 - 参数8

// 系统调用时，使用内核主堆栈
// 故设置一组函数，用于在sysret前保存和在syscall后加载
// rbp, rsp的函数
extern void save_kernel_stack();
extern void load_kernel_stack();

extern void set_kernel_stack_cache(usize stack);

#endif
