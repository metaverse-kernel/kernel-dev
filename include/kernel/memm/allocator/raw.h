#ifndef RAW_H
#define RAW_H 1

#include <types.h>
#include <kernel/memm.h>

#define MEMM_RAW_ALLOCATOR 1

typedef struct __raw_allocator_cell
{
    usize capacity;         // 是content的长度
    usize length;           // 是实际使用的长度
    allocator_t *allocator; // 所在的分配器
    usize reserved;
    u8 content[0];
} raw_allocator_cell;
#define raw_allocator_next_cell(cell) (raw_allocator_cell *)((void *)((cell)->content) + (cell)->capacity)

/**
 * @name raw_allocator_t
 * 
 * 原始分配器。包括至少一个cell，分配时像cell的分裂一样将空白的一段分成两段，释放时，只把length归零，并不将cell合并。
 * 
 * `length`为0的cell称为空cell。
 * 
 * 统计从上次细胞合并以来free的调用次数，当调用次数达到`RAW_ALLOCATOR_FREE_MAX`或无可用空间时触发细胞合并。
 * 
 * 使用建议：只在少量allocate和free的情况下使用。使用大量allocate时效率低下并难以得到内存安全保证。
 * 
 * @internal free_count
 * 
 * free方法的调用次数，达到`RAW_ALLOCATOR_FREE_MAX`时归零。
 */
typedef struct __raw_allocator_t
{
    usize size;
    usize free_count; // free调用计数
#define RAW_ALLOCATOR_FREE_MAX 64
    usize rest_memory;
    raw_allocator_cell cells[0];
} raw_allocator_t;
#define raw_allocator_end(allocator) ((void *)(allocator) + (allocator)->size)

/**
 * @name raw_allocator_new
 * 
 * ```c
 * void raw_allocator_new(raw_allocator_t *allocator, usize size);
 * ```
 * 
 * 初始化一个`raw_allocator`。
 */
void raw_allocator_new(raw_allocator_t *allocator, usize size);

/**
 * @name raw_allocator_allocate, raw_allocator_free
 * 
 * `raw_allocator`的一对allocate, free方法。
 */
void *raw_allocator_allocate(raw_allocator_t *allocator, usize size);
void raw_allocator_free(raw_allocator_t *allocator, void *mem);

#endif
