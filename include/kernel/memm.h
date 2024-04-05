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
 * 只分配不映射空间，暂时为固定值`64MB`。物理地址`0`\~`MEMM_ALLOC_ONLY_MEMORY`的空间在进入内核前已经映射至内核空间。
 *
 * 这段内存空间包含**1MB以内低地址**、**内核镜像**以及未分配空间，未分配空间被一个分配器独占。
 */
#define MEMM_ALLOC_ONLY_MEMORY (64 * 1024 * 1024)

#define MEMM_PAGE_TABLE_AREA_MAX (4 * 1024 * 1024)

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
 * @internal magic
 * 
 * 分配器有效性由此检验，不为`MEMM_ALLOCATOR_MAGIC_NUM`说明获得了一个错误的分配器地址。
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
 * @internal userspace
 *
 * 若分配器不属于内核，此成员储存此分配器的用户空间地址。
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
#ifndef MEMM_ALLOCATOR_MAGIC
#define MEMM_ALLOCATOR_MAGIC ((u32)0x271fe441)
#endif
    // 分配器有效性由此检验，不为`MEMM_ALLOCATOR_MAGIC_NUM`说明获得了一个错误的分配器地址。
    // 此值在编译时通过各种方式确定，若
    u32 magic;

    // 调用分配器的`allocate`方法后，在返回`nullptr`时会设为`true`。
    // 调用分配器的`free`方法时设为`false`。
    bool full;

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

/**
 * @name 内存管理器
 *
 * @internal alloc_only_memory
 *
 * 在进入内核主程序之前，有些不在内核中的虚拟内存空间已经被页表映射，这部分内存不可以再映射到物理页框。
 */
typedef struct __mem_manager_t
{
    usize memory_size;
    usize page_amount;

    usize alloc_only_memory;

    allocator_t *kernel_base_allocator;

    usize page_table_area;
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
 * @name memm_free
 *
 * ```c
 * void memm_free(void *mem);
 * ```
 *
 * 释放内存。
 */
void memm_free(void *mem);

void *memm_allcate_pagetable();

#endif
