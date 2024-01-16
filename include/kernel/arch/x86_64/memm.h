#ifndef X86_64_MEMM_H
#define X86_64_MEMM_H 1

#include <types.h>
#include <libk/bits.h>

/* 页大小，以MEMM_PAGE_SIZE为单位 */
#define MEMM_PAGE_SIZE 4096
typedef enum __memm_page_size
{
    MEMM_PAGE_SIZE_4K = 1,      // 1个4KB页大小
    MEMM_PAGE_SIZE_2M = 512,    // 512个4KB页大小
    MEMM_PAGE_SIZE_1G = 262144, // 262144个4KB页大小
} memm_page_size;

extern u64 PML4[512];

#define MEMM_PAGE_TABLE_FLAGS_AREA ((u64)0xfff)

/* 页对齐掩码 */
#define MEMM_4K_ALIGN_MASK ((u64)0xfff)
#define MEMM_2M_ALIGN_MASK ((u64)0x1fffff)
#define MEMM_1G_ALIGN_MASK ((u64)0x3fffffff)

/* 页表项属性FLAGS */
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
#define memm_entry_flag_get(entry, flag) \
    ((entry & flag) ? true : false)

/* 页表（大型页）项地址域掩码 */
#define MEMM_ENTRY_ADDRESS_MASK ((u64)0x000ffffffffff000)
#define MEMM_BP_ENTRY_ADDRESS_MASK ((u64)0x000fffffffffe000)
#define memm_entry_get_address(entry)                          \
    ((entry) & (memm_entry_flag_get(entry, MEMM_ENTRY_FLAG_PS) \
                    ? MEMM_BP_ENTRY_ADDRESS_MASK               \
                    : MEMM_ENTRY_ADDRESS_MASK))

/* 线性地址表项索引或页内偏移掩码 */
#define MEMM_LA_PML4EI_MASK ((u64)0x0000ff8000000000)
#define MEMM_LA_PDPTEI_MASK ((u64)0x0000007fc0000000)
#define MEMM_LA_PDEI_MASK ((u64)0x000000003fe00000)
#define MEMM_LA_PEI_MASK ((u64)0x00000000001ff000)

#define MEMM_LA_1GB_PAGE_OFFSET_MASK ((u64)0x000000003fffffff)
#define MEMM_LA_2MB_PAGE_OFFSET_MASK ((u64)0x00000000001fffff)
#define MEMM_LA_4KB_PAGE_OFFSET_MASK ((u64)0x0000000000000fff)

/* 线性地址表项索引偏移位数 */
#define MEMM_LA_PML4EI_OFFSET (39)
#define MEMM_LA_PDPTEI_OFFSET (30)
#define MEMM_LA_PDEI_OFFSET (21)
#define MEMM_LA_PEI_OFFSET (12)

/* 获取线性地址中某个表项索引以及获取页内偏移 */
#define MEMM_LA_PML4EI
#define MEMM_LA_PDPTEI
#define MEMM_LA_PDEI
#define MEMM_LA_PEI
#define memm_la_get_entry_index(addr, entry) \
    (((addr) & (entry##_MASK)) >> (entry##_OFFSET))

#define MEMM_LA_1GB_PAGE_OFFSET
#define MEMM_LA_2MB_PAGE_OFFSET
#define MEMM_LA_4KB_PAGE_OFFSET
#define memm_la_get_offset(addr, page_type) \
    ((addr) & (page_type##_MASK))

/*
仅支持canonical型地址

target与physical至少都是以MEMM_PAGE_SIZE对齐的

没有MEMM_PAGE_SIZE对齐的，和非canonical型地址都会返回false

当剩余长度超过1GB的一半且地址1GB对齐，则会映射一个1GB页；
当剩余长度超过2MB的一半且地址2MB对齐，则会映射一个2MB页；
否则映射4KB页。
 */
bool memm_map_pageframes_to(
    u64 target, u64 physical,
    usize size,
    bool user, bool write);

extern void reload_pml4();

#define is_user_address(addr) \
    (((addr) > 0xffff7fffffffffff) ? true : false)

#define is_cannonical(addr) \
    (((addr) < 0x0000800000000000 || (addr) > 0xffff7fffffffffff) ? true : false)

#define memm_get_page_align(addr)               \
    (is_aligned(addr, MEMM_PAGE_SIZE_1G)        \
         ? MEMM_PAGE_SIZE_1G                    \
         : (is_aligned(addr, MEMM_PAGE_SIZE_2M) \
                ? MEMM_PAGE_SIZE_2M             \
                : MEMM_PAGE_SIZE_4K))

#endif
