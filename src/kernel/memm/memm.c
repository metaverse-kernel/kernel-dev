#include <kernel/kernel.h>
#include <kernel/memm.h>
#include <kernel/memm/allocator/raw.h>

#include <libk/string.h>
#include <libk/bits.h>

memory_manager_t memory_manager;

memory_manager_t *memm_new(usize mem_size)
{
    memset(&memory_manager, 0, sizeof(memory_manager));
    memory_manager.memory_size = mem_size;
    memory_manager.page_amount = mem_size / MEMM_PAGE_SIZE;
    memory_manager.alloc_only_memory = MEMM_ALLOC_ONLY_MEMORY;

    usize kernel_initial_size = (usize)&kend;
    align_to(kernel_initial_size, MEMM_PAGE_SIZE);

    allocator_t *allocator0 = memm_allocator_new(
        (void *)kernel_initial_size,
        memory_manager.alloc_only_memory - MEMM_PAGE_TABLE_AREA_MAX - kernel_initial_size,
        MEMM_RAW_ALLOCATOR, 0);

    memory_manager.kernel_base_allocator = allocator0;

    return &memory_manager;
}

memory_manager_t *memm_get_manager()
{
    return &memory_manager;
}

allocator_t *memm_allocator_new(void *start, usize length, usize type, usize pid)
{
    allocator_t *allocator = start;
    allocator->magic = MEMM_ALLOCATOR_MAGIC;
    allocator->full = false;
    allocator->pid = 0;
    allocator->size = length;
    allocator->type = type;
    switch (type)
    {
    case MEMM_RAW_ALLOCATOR:
        raw_allocator_new((void *)allocator->allocator_instance, length - sizeof(allocator_t));
        allocator->allocate = (memm_allocate_t)raw_allocator_allocate;
        allocator->free = (memm_free_t)raw_allocator_free;
        break;
    default:
        allocator->magic = 0;
        break;
    }
}

void memm_allocator_destruct(allocator_t *allocator)
{
    allocator->magic = 0;
    // TODO 从分配器树中删除这个分配器
    KERNEL_TODO();
}

void *memm_kernel_allocate(usize size)
{
    allocator_t *allocator = memory_manager.kernel_base_allocator;
    return allocator->allocate(allocator->allocator_instance, size);
}

void memm_free(void *mem)
{
    allocator_t *allocator = memory_manager.kernel_base_allocator;
    if (allocator->magic != MEMM_ALLOCATOR_MAGIC)
        return;
    if (is_user_address((u64)mem))
    {
        mem = mem - allocator->userspace + (void *)allocator;
    }
    allocator->free(allocator->allocator_instance, mem);
    if (allocator->full)
        allocator->full = false;
}

void *memm_allcate_pagetable()
{
    if (memory_manager.page_table_area < MEMM_PAGE_TABLE_AREA_MAX)
    {
        memory_manager.page_table_area += MEMM_PAGE_TABLE_SIZE;
        return memory_manager.alloc_only_memory - memory_manager.page_table_area;
    }
    else
    {
        // TODO
    }
}
