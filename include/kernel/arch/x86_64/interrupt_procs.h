#ifndef X86_64_INTERRUPT_PROCS_H
#define X86_64_INTERRUPT_PROCS_H 1

#include <utils.h>

/**
 * @name interrupt_entry
 * @addindex 平台定制函数 x86_64
 *
 * 中断入口程序。
 *
 * ```c
 * void interrupt_entry();
 * ```
 *
 * ```asm
 * interrupt_entry_xx:
 *      ; 保存寄存器上下文
 *      interrupt_entry_enter
 *
 *      ; 调用真正的中断处理函数
 *      ...
 *
 *      ; 恢复寄存器上下文
 *      interrupt_entry_leave
 *      iret
 * ```
 */
typedef void (*interrupt_entry)();

/**
 * @name interrupt_entry_gen
 * @addindex 平台定制宏 x86_64
 *
 * 导入一个中断入口函数声明。
 *
 * ```c
 * #define interrupt_entry_gen(interrupt)
 * ```
 */
#define interrupt_entry_gen(interrupt) \
    extern void interrupt_entry_##interrupt()
#define interrupt_entry_sym(interrupt) \
    interrupt_entry_##interrupt

/**
 * @name interrupt_request
 * @addindex 平台定制函数
 * 
 * 中断请求处理程序。
 * 
 * ```c
 * void interrupt_request(u64 rip, u64 rsp);
 * ```
 * 
 * 由中断入口程序调用。
 */
typedef void (*interrupt_request)(u64 rip, u64 rsp, u64 errcode);

/**
 * @name interrupt_req_gen
 * @addindex 平台定制宏 x86_64
 *
 * 声明一个中断处理函数。
 *
 * ```c
 * #define interrupt_req_gen(interrupt)
 * ```
 */
#define interrupt_req_gen(interrupt) \
    void interrupt_req_##interrupt(u64 rip, u64 rsp, u64 errcode)
#define interrupt_req_sym(interrupt) \
    interrupt_req_##interrupt

#define UNSUPPORTED

#define DE
#define NMI
#define BP

interrupt_entry_gen(UNSUPPORTED);

interrupt_entry_gen(DE);
interrupt_entry_gen(NMI);
interrupt_entry_gen(BP);

#endif
