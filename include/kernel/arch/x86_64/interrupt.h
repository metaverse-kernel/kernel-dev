#ifndef X86_64_INTERRUPT_H
#define X86_64_INTERRUPT_H 1

#include <types.h>
#include <utils.h>

/**
 * @name gate_descriptor_t
 * @addindex 平台依赖结构 x86_64
 *
 * 门描述符的结构体形式。
 */
typedef struct __gate_descriptor_t
{
    u16 offset_01;
    u16 segment_selector; // for code segment
    u16 flags;
    u16 offset_23;
    u32 offset_4567;
    u32 reserved;
} DISALIGNED gate_descriptor_t;

/**
 * @name INTERRUPT_DESCRIPTOR_FLAG_IST
 * @addindex 平台依赖结构 x86_64
 *
 * `gate_descriptor_t`的`flags`的标志位。
 *
 * `ssp`代表一个tss描述符在任务段中的的索引。
 */
#define INTERRUPT_DESCRIPTOR_FLAG_IST 1

// 在第15位上有一个表示代码段是否存在的标志位，代码段总是存在，故直接设置为1
#define INTERRUPT_DESCRIPTOR_FLAG_TYPE_INTERRUPT (0x8e << 8)
#define INTERRUPT_DESCRIPTOR_FLAG_TYPE_TRAP (0x8f << 8)

/**
 * @name idt
 *
 * 中断描述符表。
 */
extern gate_descriptor_t idt[256];

#endif
