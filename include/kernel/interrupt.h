#ifndef INTERRUPT_H
#define INTERRUPT_H 1


#ifdef __x86_64__
#include <kernel/arch/x86_64/interrupt.h>
#endif

/**
 * @name interrupt_open
 * @addindex 平台定制函数
 * 
 * ```c
 * void interrupt_open();
 * ```
 * 
 * 开启中断
 */
void interrupt_open();

/**
 * @name interrupt_close
 * @addindex 平台定制函数
 * 
 * ```c
 * void interrupt_close();
 * ```
 * 
 * 开启中断
 */
void interrupt_close();

/**
 * @name interrupt_init
 * @addindex 平台定制函数
 * 
 * ```c
 * void interrupt_init();
 * ```
 * 
 * 初始化中断功能。
 */
void interrupt_init();

#endif
