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
#define raw_allocator_next_cell(cell) (raw_allocator_cell *)((void *)((cell)->content) + (cell)->capacity)

// 原始分配器
//
// 包括至少一个cell，分配时像cell的分裂一样将空白的一段分成两段，
// 释放时，只把length归零，并不将cell合并。
// length=0的cell称为空cell
//
// 统计从上次细胞合并以来free的调用次数，当调用次数很多或可用空间不足时
// 触发细胞合并。
typedef struct __raw_allocator_t
{
    usize size;
    usize free_count; // free调用计数
#define RAW_ALLOCATOR_FREE_MAX 64
    usize rest_memory;
    raw_allocator_cell cells[0];
} raw_allocator_t;
#define raw_allocator_end(allocator) ((void *)(allocator) + (allocator)->size)

void raw_allocator_new(raw_allocator_t *allocator, usize size);

void *raw_allocator_allocate(raw_allocator_t *allocator, usize size, usize align);
void raw_allocator_free(raw_allocator_t *allocator, void *mem);

#endif
