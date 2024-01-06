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
    usize rsize = size;
    align_to(rsize, 16);
    raw_allocator_cell *cell = allocator->cells;
    while ((void *)cell + cell->capacity + sizeof(raw_allocator_cell) < (void *)allocator + allocator->size)
    {
        while (
            (void *)cell + cell->capacity + sizeof(raw_allocator_cell) < (void *)allocator + allocator->size &&
            cell->length != 0)
        {
            cell = (void *)cell + cell->capacity + sizeof(raw_allocator_cell);
        }
        if (rsize <= cell->capacity)
            break;
    }
    if ((void *)cell < (void *)allocator + allocator->size)
        goto fitable_cell_finded;
    else
        return nullptr;

fitable_cell_finded:
    cell->length = size;
    if (rsize < cell->capacity)
    {
        usize cap = cell->capacity;
        cell->capacity = rsize;
        raw_allocator_cell *ncell = (void *)cell + cell->capacity + sizeof(raw_allocator_cell);
        ncell->capacity = cap - rsize - sizeof(raw_allocator_cell);
        ncell->length = 0;
    }
    return cell->content;
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
