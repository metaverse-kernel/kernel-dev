#include <kernel/memm/allocator/raw.h>

#include <kernel/kernel.h>

#include <libk/bits.h>

void raw_allocator_new(raw_allocator_t *allocator, usize size)
{
    allocator->size = size - sizeof(allocator->size);
    allocator->free_count = 0;
    allocator->rest_memory = size - sizeof(raw_allocator_t);
    allocator->cells[0].capacity = size - sizeof(raw_allocator_t) - sizeof(raw_allocator_cell);
    allocator->cells[0].length = 0;
}

void *raw_allocator_allocate(raw_allocator_t *allocator, usize size)
{
    usize real_size = size;
    align_to(real_size, 16);
    raw_allocator_cell *cell = &allocator->cells;
    while ((void *)cell < raw_allocator_end(allocator))
    {
        while ( // 确保cell指向的内容还在这个allocator内
            (void *)cell < raw_allocator_end(allocator) &&
            cell->length != 0)
        {
            cell = raw_allocator_next_cell(cell);
        }
        if (real_size <= cell->capacity)
            break;
        else
            cell = raw_allocator_next_cell(cell);
    }
    if ((void *)cell < raw_allocator_end(allocator))
        goto fitable_cell_finded;
    else
        return nullptr;

fitable_cell_finded:
    cell->length = size;
    if (real_size < cell->capacity)
    {
        usize cap = cell->capacity;
        cell->capacity = real_size;
        raw_allocator_cell *ncell = raw_allocator_next_cell(cell);
        ncell->capacity = cap - real_size - sizeof(raw_allocator_cell);
        ncell->length = 0;
    }
    allocator->rest_memory -= real_size;
    return cell->content;
}

static inline void raw_allocator_cellmerge(raw_allocator_t *allocator)
{
    raw_allocator_cell *cell = allocator->cells;
    raw_allocator_cell *ncell = raw_allocator_next_cell(cell);
    while ((void *)ncell < raw_allocator_end(allocator))
    {
        while ( // 首先保证ncell还在这个allocator内
            (void *)ncell < raw_allocator_end(allocator) &&
            cell->length == 0 && ncell->length == 0)
        {
            cell->capacity += ncell->capacity + sizeof(raw_allocator_cell);
            ncell = raw_allocator_next_cell(ncell);
        }
        cell = ncell;
        ncell = raw_allocator_next_cell(cell);
    }
}

void raw_allocator_free(raw_allocator_t *allocator, void *mem)
{
    raw_allocator_cell *cell = allocator->cells;
    while ((void *)cell < raw_allocator_end(allocator))
    { // TODO  内存错误
        if (mem == cell->content)
        {
            cell->length = 0;
            break;
        }
        cell = raw_allocator_next_cell(cell);
    }
    allocator->rest_memory += cell->capacity + sizeof(raw_allocator_cell);
    allocator->free_count++;
    if ( // 可用内存不超过当前allocator的 5% 或调用free次数很多时
        allocator->size / allocator->rest_memory > 20 ||
        allocator->free_count > RAW_ALLOCATOR_FREE_MAX)
    {
        raw_allocator_cellmerge(allocator);
        allocator->free_count = 0;
    }
}
