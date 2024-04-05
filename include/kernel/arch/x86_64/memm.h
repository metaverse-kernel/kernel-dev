#ifndef X86_64_MEMM_H
#define X86_64_MEMM_H 1

#include <types.h>
#include <libk/bits.h>

/**
 * @details x86_64
 * 
 * 物理地址空间：
 * 
 * * 0 \~ 1MB：保留。
 * * 1MB \~ 2MB：内核头。
 * * 2MB \~ 4MB：中断栈。
 * * 4MB \~ 16MB：内核栈。
 * * 16MB \~ ?：内核镜像。
 * * ? \~ 128MB：内核大分配器。
 */

/**
 * @name MEMM_PAGE_SIZE
 * @addindex 平台定制宏
 *
 * 最小的页大小。
 *
 * @if arch == x86_64 then
 *  4096
 * @endif
 */
#define MEMM_PAGE_SIZE 4096

#define MEMM_PAGE_TABLE_SIZE 4096

typedef enum __memm_page_size
{
    MEMM_PAGE_SIZE_4K = 1,      // 1个4KB页大小
    MEMM_PAGE_SIZE_2M = 512,    // 512个4KB页大小
    MEMM_PAGE_SIZE_1G = 262144, // 262144个4KB页大小
} memm_page_size;

extern u64 PML4[512];

/**
 * @name MEMM_PAGE_TABLE_FLAGS_MASK
 * @addindex 平台依赖宏 x86_64
 *
 * 页表项中属性标志位使用的最低12位的掩码`0xfff`。
 */
#define MEMM_PAGE_TABLE_FLAGS_MASK ((u64)0xfff)

/**
 * @name MEMM_xx_ALIGN_MASK
 * @addindex 平台依赖宏 x86_64
 *
 * 页对齐掩码。
 * `xx`为`x86_64`架构的页的三种大小，分别为`4K`、`2M`和`1G`。这些页一定是以它们自己的大小对齐，因此会出现低n位总为0的情况。
 * `4K`对应`0xfff`；`2M`对应`0x1fffff`；`1G`对应`0x3fffffff`。
 */
#define MEMM_4K_ALIGN_MASK ((u64)0xfff)
#define MEMM_2M_ALIGN_MASK ((u64)0x1fffff)
#define MEMM_1G_ALIGN_MASK ((u64)0x3fffffff)

/**
 * @name MEMM_ENTRY_FLAG_xx
 * @addindex 平台依赖宏 x86_64
 *
 * 页表项属性标志位。定义如下：
 *
 * ```c
 * #define MEMM_ENTRY_FLAG_PRESENT ((u64)1)
 * #define MEMM_ENTRY_FLAG_WRITE ((u64)1 << 1)
 * #define MEMM_ENTRY_FLAG_USER ((u64)1 << 2)
 * #define MEMM_ENTRY_FLAG_PWT ((u64)1 << 3)
 * #define MEMM_ENTRY_FLAG_PCD ((u64)1 << 4)
 * #define MEMM_ENTRY_FLAG_ACCECED ((u64)1 << 5)
 * #define MEMM_ENTRY_FLAG_DIRTY ((u64)1 << 6)
 * #define MEMM_ENTRY_FLAG_PS ((u64)1 << 7)
 * #define MEMM_ENTRY_FLAG_GLOBAL ((u64)1 << 8)
 * #define MEMM_ENTRY_FLAG_PAT ((u64)1 << 12)
 * #define MEMM_PTE_ENTRY_FLAG_PAT ((u64)1 << 7)
 * #define MEMM_ENTRY_FLAG_XD ((u64)1 << 63)
 * ```
 *
 * 其中`MEMM_PTE_ENTRY_FLAG_PAT`的`pte`表示此属性标志位只存在与`pte`页表项中。
 */
#define MEMM_ENTRY_FLAG_PRESENT ((u64)1)
#define MEMM_ENTRY_FLAG_WRITE ((u64)1 << 1)
#define MEMM_ENTRY_FLAG_USER ((u64)1 << 2)
#define MEMM_ENTRY_FLAG_PWT ((u64)1 << 3)
#define MEMM_ENTRY_FLAG_PCD ((u64)1 << 4)
#define MEMM_ENTRY_FLAG_ACCECED ((u64)1 << 5)
#define MEMM_ENTRY_FLAG_DIRTY ((u64)1 << 6)
#define MEMM_ENTRY_FLAG_PS ((u64)1 << 7)
#define MEMM_ENTRY_FLAG_GLOBAL ((u64)1 << 8)
#define MEMM_ENTRY_FLAG_PAT ((u64)1 << 12)
#define MEMM_PTE_ENTRY_FLAG_PAT ((u64)1 << 7)
#define MEMM_ENTRY_FLAG_XD ((u64)1 << 63)

/**
 * @name memm_entry_flag_get(entry, flag)
 * @addindex 平台依赖宏 x86_64
 *
 * 获取页表项中某个属性标志位，得到布尔值。
 *
 * 其中`flag`是`MEMM_ENTRY_FLAG_xx`。
 */
#define memm_entry_flag_get(entry, flag) \
    ((entry & flag) ? true : false)

/**
 * @name MEMM_ENTRY_ADDRESS_MASK, MEMM_BP_ENTRY_ADDRESS_MASK
 * @addindex 平台依赖宏 x86_64
 *
 * 页表项地址域掩码。将页表项与掩码作与运算可得到页表项指向的**下一级页表的起始地址**或**页框的起始地址**。
 *
 * 含`BP`的为大型页页表项的地址域掩码。
 *
 * 定义如下：
 *
 * ```c
 * #define MEMM_ENTRY_ADDRESS_MASK ((u64)0x000ffffffffff000)
 * #define MEMM_BP_ENTRY_ADDRESS_MASK ((u64)0x000fffffffffe000)
 * ```
 */
#define MEMM_ENTRY_ADDRESS_MASK ((u64)0x000ffffffffff000)
#define MEMM_BP_ENTRY_ADDRESS_MASK ((u64)0x000fffffffffe000)

/**
 * @name memm_entry_get_address(entry)
 * @addindex 平台依赖宏 x86_64
 *
 * 获取页表项指向的地址。
 */
#define memm_entry_get_address(entry)                          \
    ((entry) & (memm_entry_flag_get(entry, MEMM_ENTRY_FLAG_PS) \
                    ? MEMM_BP_ENTRY_ADDRESS_MASK               \
                    : MEMM_ENTRY_ADDRESS_MASK))

#define MEMM_LA_PML4EI_MASK ((u64)0x0000ff8000000000)
#define MEMM_LA_PDPTEI_MASK ((u64)0x0000007fc0000000)
#define MEMM_LA_PDEI_MASK ((u64)0x000000003fe00000)
#define MEMM_LA_PEI_MASK ((u64)0x00000000001ff000)

#define MEMM_LA_1GB_PAGE_OFFSET_MASK ((u64)0x000000003fffffff)
#define MEMM_LA_2MB_PAGE_OFFSET_MASK ((u64)0x00000000001fffff)
#define MEMM_LA_4KB_PAGE_OFFSET_MASK ((u64)0x0000000000000fff)

#define MEMM_LA_PML4EI_OFFSET (39)
#define MEMM_LA_PDPTEI_OFFSET (30)
#define MEMM_LA_PDEI_OFFSET (21)
#define MEMM_LA_PEI_OFFSET (12)

/**
 * @name MEMM_LA_xxxxI
 * @addindex 平台依赖宏 x86_64
 *
 * 在4级分页中，线性地址的四个页表项索引宏。其中`xxxx`分别为`PML4E`、`PDPTE`、`PDE`和`PE`。
 */
#define MEMM_LA_PML4EI
#define MEMM_LA_PDPTEI
#define MEMM_LA_PDEI
#define MEMM_LA_PEI

/**
 * @name memm_la_get_entry_index(addr, entry)
 * @addindex 平台依赖宏 x86_64
 *
 * 获取线性地址`addr`中的`entry`页表项索引。
 *
 * 其中`entry`应为宏`MEMM_LA_xxxxI`。
 */
#define memm_la_get_entry_index(addr, entry) \
    (((addr) & (entry##_MASK)) >> (entry##_OFFSET))

/**
 * @name MEMM_LA_xxx_PAGE_OFFSET
 * @addindex 平台依赖宏 x86_64
 *
 * 线性地址的页内偏移宏。其中`xxx`为三种页容量`4KB`、`2MB`、`1GB`。
 */
#define MEMM_LA_1GB_PAGE_OFFSET
#define MEMM_LA_2MB_PAGE_OFFSET
#define MEMM_LA_4KB_PAGE_OFFSET

/**
 * @name memm_la_get_offset(addr, page_type)
 * @addindex 平台依赖宏 x86_64
 *
 * 获取线性地址的页内偏移部分。
 *
 * 其中`page_type`应为宏`MEMM_LA_xxx_PAGE_OFFSET`。
 */
#define memm_la_get_offset(addr, page_type) \
    ((addr) & (page_type##_MASK))

/**
 * @name memm_map_pageframes_to
 *
 * ```c
 * bool memm_map_pageframes_to(
 *     u64 target, u64 physical,
 *     usize size,
 *     bool user, bool write);
 * ```
 *
 * 仅支持**canonical**型地址
 *
 * `target`与`physical`保证至少都是以MEMM_PAGE_SIZE对齐的。
 *
 * 没有MEMM_PAGE_SIZE对齐的，和非canonical型地址都会返回false
 *
 * 当剩余长度超过1GB的一半且地址1GB对齐，则会映射一个1GB页；
 * 当剩余长度超过2MB的一半且地址2MB对齐，则会映射一个2MB页；
 * 否则映射4KB页。
 */
bool memm_map_pageframes_to(
    u64 target, u64 physical,
    usize size,
    bool user, bool write);

/**
 * @name reload_pml4
 * @addindex 平台依赖宏 x86_64
 *
 * ```c
 * void reload_pml4();
 * ```
 */
extern void reload_pml4();

/**
 * @name is_user_address(addr)
 * @addindex 平台定制宏
 *
 * 判断`addr`是否是一个用户空间地址，得到一个布尔值。
 *
 * @if arch == x86_64
 *  `canonical`型地址的高地址为用户空间，低地址为内核空间。
 * @endif
 */
#define is_user_address(addr) \
    (((addr) > 0xffff7fffffffffff) ? true : false)

/**
 * @name is_cannonical(addr)
 * @addindex 平台依赖宏 x86_64
 *
 * 判断`addr`是否是一个cannonical型地址。
 */
#define is_cannonical(addr) \
    (((addr) < 0x0000800000000000 || (addr) > 0xffff7fffffffffff) ? true : false)

/**
 * @name memm_get_page_align(addr)
 * @addindex 平台依赖宏 x86_64
 *
 * 获取地址`addr`的页对齐大小，得到一个`memm_page_size`类型值。
 *
 * 当地址`addr`是*4KB对齐*或*没有4KB对齐时*，都得到`MEMM_PAGE_SIZE_4K`。
 */
#define memm_get_page_align(addr)               \
    (is_aligned(addr, MEMM_PAGE_SIZE_1G)        \
         ? MEMM_PAGE_SIZE_1G                    \
         : (is_aligned(addr, MEMM_PAGE_SIZE_2M) \
                ? MEMM_PAGE_SIZE_2M             \
                : MEMM_PAGE_SIZE_4K))

/**
 * @name memm_page_counter
 * @addindex 平台定制结构
 * 
 * 页计数器
 * 
 * @if arch == x86_64
 *  使用三个成员分别记录`4KB`、`2MB`、`1GB`页的大小。
 * @endif
 */
typedef struct __memm_page_counter
{
    usize mapped_4k_page;
    usize mapped_2m_page;
    usize mapped_1g_page;
} memm_page_counter;

#endif
