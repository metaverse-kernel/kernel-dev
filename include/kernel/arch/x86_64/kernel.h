#ifndef X86_64_KERNEL_H
#define X86_64_KERNEL_H 1

#include <types.h>

// 具有返回值是为了留出一个寄存器用于调整栈顶
extern usize prepare_stack();

#endif
