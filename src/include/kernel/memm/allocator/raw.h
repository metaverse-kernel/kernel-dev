#ifndef RAW_H
#define RAW_H 1

#include <types.h>

#define MEMM_RAW_ALLOCATOR 1

typedef struct __raw_allocator_cell
{
    usize capacity; // 是content的长度
    usize length;   // 是实际使用的长度
    u8 content[0];
} raw_allocator_cell;

// 原始分配器
// 包括至少一个cell，分配时像cell的分裂一样将空白的一段分成两段
// 释放时，只把length归零，并不将cell合并
// length=0的cell称为空cell
// 在分配时遇到第一个空cell时将紧随其后的空cell都合并掉
typedef struct __raw_allocator_t
{
    usize size, reserved;
    raw_allocator_cell cells[0];
} raw_allocator_t;

void raw_allocator_new(raw_allocator_t *allocator, usize size);

void *raw_allocator_allocate(raw_allocator_t *allocator, usize size, usize align);
void raw_allocator_free(raw_allocator_t *allocator, void *mem);

#endif
