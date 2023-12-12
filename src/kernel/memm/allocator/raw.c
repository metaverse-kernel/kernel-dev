#include <kernel/memm/allocator/raw.h>

#include <kernel/kernel.h>

#include <libk/bits.h>

void raw_allocator_new(raw_allocator_t *allocator, usize size)
{
    allocator->size = size - sizeof(allocator->size);
    allocator->cells[0].capacity = size - sizeof(raw_allocator_t);
    allocator->cells[0].length = 0;
}

void *raw_allocator_allocate(raw_allocator_t *allocator, usize size, usize align)
{
    // 搜索
    raw_allocator_cell *cell = allocator->cells;
    while ((void *)cell < (void *)allocator + allocator->size &&
           cell->length != 0 && cell->capacity >= size)
    {
        cell = (void *)cell + sizeof(raw_allocator_cell) + cell->capacity;
    }
    if ((void *)cell >= (void *)allocator + allocator->size)
        return nullptr;

    // 合并
    raw_allocator_cell *next_cell = (void *)cell + sizeof(raw_allocator_cell) + cell->capacity;
    while ((void *)next_cell < (void *)allocator + allocator->size &&
           next_cell->length == 0)
    {
        cell->capacity += sizeof(raw_allocator_cell) + next_cell->capacity;
        next_cell = (void *)next_cell + sizeof(raw_allocator_cell) + next_cell->capacity;
    }

    // 分割
    usize allsize = sizeof(raw_allocator_cell) + size;
    align_to(allsize, 16);
    next_cell = (void *)cell + allsize;
    if (allsize + sizeof(raw_allocator_cell) < sizeof(raw_allocator_cell) + cell->capacity)
    {
        usize nextsize = sizeof(raw_allocator_cell) + cell->capacity - allsize;
        next_cell->capacity = nextsize - sizeof(raw_allocator_cell);
        next_cell->length = 0;
    }
    else
    {
        allsize += 16;
    }

    // 分配
    cell->capacity = allsize - sizeof(raw_allocator_cell);
    cell->length = size;

    return &cell->content;
}

void raw_allocator_free(raw_allocator_t *allocator, void *mem)
{
    raw_allocator_cell *cell = allocator->cells;
    while ((void *)cell < (void *)allocator + allocator->size)
    {
        if (mem == cell->content)
        {
            cell->length = 0;
            break;
        }
        cell = (void *)cell + sizeof(raw_allocator_cell) + cell->capacity;
    }
}
