#ifndef MEMM_H
#define MEMM_H 1

#ifdef __x86_64__
#include <kernel/arch/x86_64/memm.h>
#endif

#include <libk/lst.h>

/**
 * @brief 内存管理模块
 *
 * TODO 还没设计页回收算法
 */

/* 最大支持1TB内存 */
#define MEMM_MAX_SUPPORTED_MEMORY (1024 * (1024 * (1024 * (usize)1024)))

/* 最大支持的分页数量 */
// 这里的页均以最小的页大小计算
#define MEMM_MAX_SUPPORTED_PAGES (MEMM_MAX_SUPPORTED_MEMORY / MEMM_PAGE_SIZE)

/* 只分配不映射空间 */
#define MEMM_ALLOC_ONLY_MEMORY (128 * 1024 * 1024)

typedef void *(*memm_allocate_t)(void *allocator, usize size, usize align);
typedef void (*memm_free_t)(void *allocator, void *mem);

/*
内存分配器
分配器对象的首地址永远是MEMM_PAGE_SIZE对齐的
 */
typedef struct __allocator_t
{
    bool initialized;

    // 在本分配器中调用allocate返回nullptr后为true
    // 调用free后为false
    bool full;

    // 进程id，当pid=0时代表内核
    usize pid;
    // 分配器类型
    usize type;
    usize size;

    // 分配器实例的allocate函数
    // 无法分配空间返回nullptr
    // 在size参数为0时，保证不可以分配空间，但是如果空间已满依然返回nullptr
    // 当参数align=0时表示不需要对齐
    memm_allocate_t allocate;

    // 分配器实例的free函数
    // 若不是allocate得到的地址则什么都不做
    memm_free_t free;

    // 分配器实例
    // 对应`type`类型使用
    // 在kernel/memm/allocator/中是所有的内存分配器
    u64 allocator_instance[0];
} allocator_t;

typedef struct __allocator_iterator_t
{
    allocator_t *allocator;
    struct __allocator_iterator_t *left, *right;
} allocator_iterator_t;

/*
内存管理器
内存分配分成两个阶段：映射和分配
映射后的页进入allocator记录中，具体的分配工作由allocator完成

在page_map中置位、在map_with_allocator中复位、在map_with_destructed_allocator中复位
且不在available_pages_table中的页，是页表、程序代码段、内核代码等使用的页
*/
typedef struct __mem_manager_t
{
    usize memory_size;
    usize page_amount;

    // 这里记录的数量为最小的页的数量
    usize mapped_page_amount;

    // 在进入内核主程序之前，有些不在内核中的虚拟内存空间已经被
    // 页表映射，这部分内存不可以再映射到物理页框
    usize alloc_only_memory;
#ifdef __x86_64__
    // 在这里三种页的数量分别记录
    usize mapped_4k_page;
    usize mapped_2m_page;
    usize mapped_1g_page;
#endif
    // 页地图
    // 每个bit都表示这个页是否被映射
    u8 *page_map;
    // 分配器页地图
    // 每个bit表示这个页是否被内存分配器控制
    // （不代表每个页都包含完整的分配器）
    u8 *map_with_allocator;
    // 分配器释放页地图
    // 每个bit表示这个页是否曾经被内存分配器控制且现在被释放
    // 需要取消映射
    u8 *map_with_destructed_allocator;

    // 空闲页线段搜索表
    lst_iterator_t *available_pages_table;

    // 分配器树
    // 为方便查找，以二叉树的形式存储
    // index=0为无效项
    allocator_iterator_t *allocators;
} mem_manager_t;

mem_manager_t *memm_new(usize mem_size);

/*
在`start`处创建一个长度为`length`的内存分配器
当`pid`为0时，表示这个分配器只给内核使用
 */
allocator_t *memm_allocator_new(void *start, usize length, usize type, usize pid);

/*
释放分配器对象

只能被内存回收模块调用
 */
void memm_allocator_destruct(allocator_t *allocator);

/*
申请内存
第三个参数也是返回值，表示申请内存使用的分配器
pid=0时为内核分配
所有内存在内核空间都有对物理内存空间的直接映射，也就是线性地址与物理地址相同，称为内核地址

allocator对象在进程与内核之间传递时一律使用内核空间的映射地址

## 要求在返回的地址前16字节处保留8字节空间作为它所在的allocator地址，并且不需要分配器的具体实现做这个事
 */
void *memm_allocate(usize size, usize pid);
#define memm_addr_set_allocator(mem, allocator) \
    *(allocator_t **)(mem - 16) = allocator;
#define memm_addr_get_allocator(mem) \
    ((*(allocator_t **)(mem - 16)))

void *memm_kernel_allocate(usize size);

void *memm_user_allocate(usize size, usize pid);

/*
释放内存
 */
void memm_free(void *mem);

/*
寻找大小合适的一组页
 */
usize find_fitable_pages(usize page_count);

#endif
