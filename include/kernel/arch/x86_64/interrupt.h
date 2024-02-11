#ifndef X86_64_INTERRUPT_H
#define X86_64_INTERRUPT_H 1

#include <types.h>
#include <utils.h>

typedef struct __gate_descriptor_t
{
    u16 offset_01;
    u16 segment_selector; // for code segment
    u16 flags;
    u16 offset_23;
    u32 offset_4567;
    u32 reserved;
} DISALIGNED gate_descriptor_t;

// interrupt stack table，每个表项都指向tss
// 需要加载寄存器IA32_INTERRUPT_SSP_TABLE
#define INTERRUPT_DESCRIPTOR_FLAG_IST(ssp) (ssp)

// 在第15位上有一个表示代码段是否存在的标志位，代码段总是存在，故直接设置为1
#define INTERRUPT_DESCRIPTOR_FLAG_TYPE_INTERRUPT (0x8e << 8)
#define INTERRUPT_DESCRIPTOR_FLAG_TYPE_TRAP (0x8f << 8)

extern gate_descriptor_t idt[256];

#endif
