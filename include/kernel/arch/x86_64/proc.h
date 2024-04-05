#ifndef X86_64_PROC_H
#define X86_64_PROC_H 1

#include <types.h>

/**
 * @name proc_texture_registers_t
 * @addindex 平台定制宏 x86_64
 * 
 * 进程上下文的寄存器上下文。
 */
typedef struct __proc_texture_registers_t
{
    u16 es, ds;
    u32 reserved;
    u64 r15, r14, r13, r12;
    u64 r11, r10, r9, r8;
    u64 rdi, rsi, rdx, rbx, rcx, rax;
    u64 rbp;
    u64 rip, cs;
    u64 rflags;
    u64 rsp, ss;
} proc_texture_registers_t;

#endif
