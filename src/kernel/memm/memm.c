#include <kernel/kernel.h>
#include <kernel/memm.h>
#include <kernel/memm/allocator/raw.h>

#include <libk/string.h>
#include <libk/bits.h>

mem_manager_t memory_manager;

mem_manager_t *memm_new(usize mem_size)
{
    memset(&memory_manager, 0, sizeof(memory_manager));
    memory_manager.memory_size = mem_size;
    memory_manager.page_amount = mem_size / MEMM_PAGE_SIZE;
    memory_manager.alloc_only_memory = MEMM_ALLOC_ONLY_MEMORY;

    usize kernel_initial_size = (usize)&kend;
    align_to(kernel_initial_size, MEMM_PAGE_SIZE);

    // 配置分配器树
    allocator_t *allocator0 = memm_allocator_new(
        (void *)kernel_initial_size,
        memory_manager.alloc_only_memory - kernel_initial_size,
        MEMM_RAW_ALLOCATOR, 0);

    allocator_iterator_t *alcatr_ind = allocator0->allocate(
        &allocator0->allocator_instance, sizeof(allocator_iterator_t), 0);

    alcatr_ind->allocator = allocator0;
    alcatr_ind->left = nullptr;
    alcatr_ind->right = nullptr;

    memory_manager.allocators = alcatr_ind;

    // 配置页映射地图
    usize pmc_size = memory_manager.page_amount;
    align_to(pmc_size, 8);
    pmc_size /= 8;

    memory_manager.page_map = allocator0->allocate(&allocator0->allocator_instance, pmc_size, 0);
    memset(memory_manager.page_map, 0, pmc_size);
    memset(memory_manager.page_map, 0xff, MEMM_ALLOC_ONLY_MEMORY / MEMM_PAGE_SIZE / 8);
    for (usize i = (MEMM_ALLOC_ONLY_MEMORY / MEMM_PAGE_SIZE / 8) * (u8)8;
         i < MEMM_ALLOC_ONLY_MEMORY / MEMM_PAGE_SIZE % 8; i++)
    {
        bitmap_set(memory_manager.page_map, i);
    }

    // 配置分配器页地图
    memory_manager.map_with_allocator =
        allocator0->allocate(&allocator0->allocator_instance, pmc_size, 0);
    memset(memory_manager.map_with_allocator, 0, pmc_size);
    for (usize i = kernel_initial_size / MEMM_PAGE_SIZE;
         i < MEMM_ALLOC_ONLY_MEMORY / MEMM_PAGE_SIZE;
         i += MEMM_PAGE_SIZE)
    {
        bitmap_set(memory_manager.map_with_allocator, i);
    }

    // 分配器释放页地图
    memory_manager.map_with_destructed_allocator =
        allocator0->allocate(&allocator0->allocator_instance, pmc_size, 0);
    memset(memory_manager.map_with_destructed_allocator, 0, pmc_size);

    // 配置空闲页线段搜索表
    memory_manager.available_pages_table = lst_new(0, memory_manager.page_amount);
    lst_remove(memory_manager.available_pages_table, 0, MEMM_ALLOC_ONLY_MEMORY / MEMM_PAGE_SIZE, false);

    return &memory_manager;
}

allocator_t *memm_allocator_new(void *start, usize length, usize type, usize pid)
{
    allocator_t *allocator = start;
    allocator->initialized = true;
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
        allocator->initialized = false;
        break;
    }
}

void memm_allocator_destruct(allocator_t *allocator)
{
    allocator->initialized = false;
    // TODO 从分配器树中删除这个分配器
    KERNEL_TODO();
}

void *memm_find_and_allocate(allocator_iterator_t *allocator_ind, usize size, usize pid, allocator_t **writeback)
{
    void *ptr;
    allocator_t *allocator = allocator_ind->allocator;
    if (allocator->pid == pid && allocator->full == false)
    { // 尝试用本节点分配
        if ((ptr = allocator->allocate(&allocator->allocator_instance, size, 0)) != nullptr)
        {
            *writeback = allocator;
            return ptr;
        }
        else
        {
            if ((ptr = allocator->allocate(&allocator->allocator_instance, 0, 0)) == nullptr)
                allocator->full = true;
        }
    }
    if (allocator_ind->left != nullptr)
    { // 尝试用左子树分配
        ptr = memm_find_and_allocate(allocator_ind->left, size, pid, writeback);
        if (ptr != nullptr)
            return ptr;
    }
    if (allocator_ind->right != nullptr)
    { // 尝试用右子树分配
        ptr = memm_find_and_allocate(allocator_ind->right, size, pid, writeback);
        if (ptr != nullptr)
            return ptr;
    }
    // 都不行就只能返回nullptr
    return nullptr;
}

static void insert_allocator(allocator_iterator_t *iter, allocator_iterator_t *inserter)
{
    if (inserter->allocator < iter->allocator)
    {
        if (iter->left == nullptr)
        {
            iter->left = inserter;
            return;
        }
        else
        {
            insert_allocator(iter->left, inserter);
        }
    }
    else if (inserter->allocator > iter->allocator)
    {
        if (iter->right == nullptr)
        {
            iter->right = inserter;
            return;
        }
        else
        {
            insert_allocator(iter->right, inserter);
        }
    }
}

void *memm_allocate(usize size, usize pid)
{
    usize orgsize = size;
    // 从分配器树中分配内存
    allocator_t *allocator;
    void *ptr = memm_find_and_allocate(memory_manager.allocators, size, pid, &allocator);
    if (ptr != nullptr)
        goto after_allocation;

    // 分配器树中没有可分配的内存
    size += sizeof(allocator_t) + MEMM_PAGE_SIZE;
    align_to(size, MEMM_PAGE_SIZE);
    size /= MEMM_PAGE_SIZE;
    usize allocator_start = find_fitable_pages(size);
    if (allocator_start == 0)
        return nullptr; // 内存中已经没有可分配的页了
    for (usize i = allocator_start; i < allocator_start + size; i++)
    {
        bitmap_set(memory_manager.map_with_allocator, i);
    }
    memm_map_pageframes_to(
        allocator_start * MEMM_PAGE_SIZE, allocator_start * MEMM_PAGE_SIZE,
        size * MEMM_PAGE_SIZE,
        false, // 用户空间标志
        true   // 写权限
    );

    // 在新映射的页中创建一个分配器
    // TODO 在用户态可能需要实现一个效率更高的分配器
    allocator_t *new_allocator =
        memm_allocator_new((void *)(allocator_start * MEMM_PAGE_SIZE), size * MEMM_PAGE_SIZE,
                           MEMM_RAW_ALLOCATOR, pid);
    allocator = new_allocator;

    allocator_iterator_t *allind = memm_kernel_allocate(sizeof(allocator_iterator_t));
    allind->allocator = new_allocator;
    allind->left = nullptr;
    allind->right = nullptr;
    insert_allocator(memory_manager.allocators, allind);
    ptr = new_allocator->allocate(&new_allocator->allocator_instance, orgsize, 0);

after_allocation:
    if (ptr != nullptr)
        memm_addr_set_allocator(ptr, allocator);
    return ptr;
}

void *memm_kernel_allocate(usize size)
{
    return memm_allocate(size, 0);
}

void *memm_user_allocate(usize size, usize pid)
{
    void *res = memm_allocate(size, pid);
    // TODO 将内存空间映射到用户空间
    return res;
}

void memm_free(void *mem)
{
    allocator_t *allocator = memm_addr_get_allocator(mem);
    if (is_user_address((u64)mem))
    { // TODO 对于用户空间的地址需要先转换到内核地址后释放
    }
    allocator->free(allocator->allocator_instance, mem);
    if (allocator->full)
        allocator->full = false;
}

usize find_fitable_pages(usize page_count)
{
    usize res = 0;
    lst_iterator_t *iter = memory_manager.available_pages_table;
    do
    {
        if (iter->line.right - iter->line.left > page_count)
        {
            res = iter->line.left;
            lst_remove(iter, res, res + page_count, false);
            for (usize i = res; i < res + page_count; i++)
            {
                bitmap_set(memory_manager.page_map, i);
            }
            break;
        }
    } while ((iter = lst_next(iter)) != nullptr);
    memory_manager.mapped_page_amount += page_count;
    return res;
}
