#ifndef MEMM_H
#define MEMM_H 1

#ifdef __x86_64__
#include <kernel/arch/x86_64/memm.h>
#endif

#include <libk/lst.h>

/**
 * @name MEMM_MAX_SUPPORTED_MEMORY
 *
 * 支持的最大物理内存，为固定值`1TB`。
 */
#define MEMM_MAX_SUPPORTED_MEMORY (1024 * (1024 * (1024 * (usize)1024)))

/**
 * @name MEMM_MAX_SUPPORTED_PAGES
 *
 * 支持的最大页数量，通过宏`MEMM_MAX_SUPPORTED_MEMORY`与宏`MEMM_PAGE_SIZE`相除得到。
 */
#define MEMM_MAX_SUPPORTED_PAGES (MEMM_MAX_SUPPORTED_MEMORY / MEMM_PAGE_SIZE)

/**
 * @name MEMM_ALLOC_ONLY_MEMORY
 *
 * 只分配不映射空间，暂时为固定值`128MB`。物理地址`0`\~`MEMM_ALLOC_ONLY_MEMORY`的空间在进入内核前已经映射至内核空间。
 *
 * 这段内存空间包含**1MB以内低地址**、**内核镜像**以及未分配空间，未分配空间被一个分配器独占。
 */
#define MEMM_ALLOC_ONLY_MEMORY (128 * 1024 * 1024)

/**
 * @name memm_allocate_t, memm_free_t
 *
 * 分配器的一对`分配`、`释放`函数指针。内核中可以根据情况使用多种不同的分配器来分配内存，`allocator`中提供一对此类型的变量，用于
 * 动态绑定不同的分配器。
 * 
 * 分配器在`kernel/memm/allocator/`中定义。
 * 
 * 分配器**必须**实现的行为：
 *
 * **`memm_allocate_t`**：
 *
 * ```c
 * typedef void *(*memm_allocate_t)(void *allocator, usize size);
 * ```
 *
 * 从分配器`allocator`中分配`size`字节的内存。
 *
 * 无法分配空间时返回`nullptr`；当参数`size`为0时，需要检查分配器占有的空间的可用性，无可用空间时需要返回`nullptr`；
 * **返回的地址的16字节前**必须保留8字节空间用于存放内存所在分配器的内核空间地址。
 *
 * **`memm_free_t`**：
 *
 * ```c
 * typedef void (*memm_free_t)(void *allocator, void *mem);
 * ```
 *
 * 释放分配器`allocator`中的内存`mem`。
 *
 * 由于此函数只由内核调用，因此对参数作出保证，保证地址`mem`属于这个`allocator`。
 */
typedef void *(*memm_allocate_t)(void *allocator, usize size);
typedef void (*memm_free_t)(void *allocator, void *mem);

/**
 * @name allocator_t
 *
 * ```c
 * typedef struct { } allocator_t;
 * ```
 *
 * 内存分配器。
 *
 * 内存分配器用于为`memm_allocate`函数提供管理数据以及分配空间。
 *
 * 在**内核镜像结尾**至`MEMM_ALLOC_ONLY_MEMORY`空间中，包含一个分配器`内核大分配器`。
 *
 * 分配器指针**必须**使用内核地址。
 *
 * @internal full
 *
 * 调用分配器的`allocate`方法后，在返回`nullptr`时会设为`true`。
 * 调用分配器的`free`方法时设为`false`。
 *
 * @internal pid
 *
 * 进程标志服，表示此分配器所属的进程，为0代表属于内核。
 *
 * @internal type
 *
 * 分配器类型。在目录`include/kernel/memm/allocator`中对每个分配器分别定义一个唯一值。
 *
 * @internal allocate
 *
 * 分配器实例的allocate函数。当无法分配空间时返回nullptr。在size参数为0时，保证不可以分配空间，
 * 但是如果空间已满依然返回nullptr。
 *
 * @internal free
 *
 * 分配器实例的free函数。若不是allocate得到的地址则什么都不做。
 *
 * @internal allocator_instance
 *
 * 分配器实例。
 */
typedef struct __allocator_t
{
#define MEMM_ALLOCATOR_MAGIC_NUM 0x271fe441
    u32 magic;

    bool initialized;

    // 调用分配器的`allocate`方法后，在返回`nullptr`时会设为`true`。
    // 调用分配器的`free`方法时设为`false`。
    bool full;

    // 进程标志服，表示此分配器所属的进程，为0代表属于内核。
    usize pid;
    // 分配器类型。在目录`include/kernel/memm/allocator`中对每个分配器分别定义一个唯一值。
    usize type;
    usize size;

    // 分配器实例的allocate函数。当无法分配空间时返回nullptr。在size参数为0时，
    // 保证不可以分配空间，但是如果空间已满依然返回nullptr。
    memm_allocate_t allocate;
    // 分配器实例的free函数。若不是allocate得到的地址则什么都不做。
    memm_free_t free;

    // 分配器实例。
    u64 allocator_instance[0];
} allocator_t;

typedef struct __allocator_iterator_t
{
    allocator_t *allocator;
    struct __allocator_iterator_t *left, *right;
} allocator_iterator_t;

/**
 * @name 内存管理器
 *
 * @internal alloc_only_memory
 *
 * 在进入内核主程序之前，有些不在内核中的虚拟内存空间已经被页表映射，这部分内存不可以再映射到物理页框。
 *
 * @internal mapped_page_amount
 *
 * 已经映射的页数量。若不是最小的页会被视作多个最小页计数。
 *
 * @internal mapped_4k_page, mapped_2m_page, mapped_1g_page
 * @addindex 平台依赖宏 x86_64
 *
 * 分别记录已经映射的三种大小页的数量。
 *
 * @internal page_map
 *
 * 页地图，每个bit表示对应的最小页是否被映射。
 *
 * @internal allocator_map
 *
 * 分配器地图。每个bit表示对应的最小页是否被一个分配器控制。
 *
 * @internal destructed_allocator_map
 *
 * 释放的分配器页地图。每个bit表示对应的最小页是否曾经被分配器控制并且现在控制这个页的分配器已经释放。
 *
 * 值为1的bit位对应的最小页可以直接**取消映射**、**重新构造一个分配器**、**加载可执行程序**等。
 *
 * @internal available_pages_table
 *
 * 空闲页线段搜索表。
 *
 * @internal allocators
 *
 * 分配器二叉树。
 */
typedef struct __mem_manager_t
{
    usize memory_size;
    usize page_amount;

    // 在进入内核主程序之前，有些不在内核中的虚拟内存空间已经被页表映射，这部分内存不可以再映射到物理页框
    usize alloc_only_memory;

    // 已经映射的页数量。若不是最小的页会被视作多个最小页计数。
    usize mapped_page_amount;
    memm_page_counter platformed_page_counter;

    // 页地图。每个bit都表示这个页是否被映射。
    u8 *page_map;
    // 分配器页地图。每个bit表示这个页是否被内存分配器控制。
    u8 *allocator_map;
    // 释放的分配器页地图。每个bit表示这个页是否曾经被内存分配器控制且现在被释放。
    // 值为1的bit位对应的最小页可以直接**取消映射**、**重新构造一个分配器**、**加载可执行程序**等。
    u8 *destructed_allocator_map;

    // 空闲页线段搜索表
    lst_iterator_t *available_pages_table;

    // 分配器二叉树
    allocator_iterator_t *allocators;
} memory_manager_t;

/**
 * @name memm_new
 *
 * ```c
 * memory_manager_t *memm_new(usize mem_size);
 * ```
 *
 * 初始化内存管理结构。
 */
memory_manager_t *memm_new(usize mem_size);

/**
 * @name memm_get_manager
 *
 * ```c
 * memory_manager_t *memm_get_manager();
 * ```
 *
 * 在其它源代码文件中，获取内存管理结构的方式。
 */
memory_manager_t *memm_get_manager();

/**
 * @name memm_allocator_new
 *
 * ```c
 * allocator_t *memm_allocator_new(void *start, usize length, usize type, usize pid);
 * ```
 *
 * 在`start`处创建一个长度为`length`的内存分配器，当`pid`为0时，表示这个分配器只给内核使用。
 */
allocator_t *memm_allocator_new(void *start, usize length, usize type, usize pid);

/**
 * @name memm_allocator_destruct
 *
 * ```c
 * void memm_allocator_destruct(allocator_t *allocator);
 * ```
 *
 * 释放分配器对象。
 *
 * 只能被**内存回收模块**调用。
 */
void memm_allocator_destruct(allocator_t *allocator);

/**
 * @name memm_allocate
 *
 * ```c
 * void *memm_allocate(usize size, usize pid);
 * ```
 *
 * 申请内存。`pid`为0时为内核分配。
 *
 * 所有内存在内核空间都有对物理内存空间的直接映射。
 */
void *memm_allocate(usize size, usize pid);
#define memm_addr_set_allocator(mem, allocator) \
    *(allocator_t **)((void *)(mem)-16) = allocator;
#define memm_addr_get_allocator(mem) \
    ((*(allocator_t **)((void *)(mem)-16)))

/**
 * @name memm_kernel_allocate
 *
 * ```c
 * void *memm_kernel_allocate(usize size);
 * ```
 *
 * 为内核空间申请内存。
 */
void *memm_kernel_allocate(usize size);

/**
 * @name memm_user_allocate
 *
 * ```c
 * void *memm_user_allocate(usize size, usize pid);
 * ```
 *
 * 为用户空间申请内存。
 */
void *memm_user_allocate(usize size, usize pid);

/**
 * @name memm_free
 * 
 * ```c
 * void memm_free(void *mem);
 * ```
 * 
 * 释放内存。
 */
void memm_free(void *mem);

/**
 * @name find_fitable_pages
 * 
 * ```c
 * usize find_fitable_pages(usize page_count);
 * ```
 * 寻找大小合适的一组连续页
 */
usize find_fitable_pages(usize page_count);

#endif
