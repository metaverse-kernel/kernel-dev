#ifndef MM_H
#define MM_H 1

#include <types.h>

#define MM_PAGE_TABLE_FLAGS_AREA ((u64)0xfff)

/* 页对齐掩码 */

#define MM_4K_ALIGN_MASK ((u64)0xfff)
#define MM_2M_ALIGN_MASK ((u64)0x1fffff)
#define MM_1G_ALIGN_MASK ((u64)0x3fffffff)

/* 页表项属性FLAGS */

#define MM_PML4E_KERNEL_RW_FLAG ((u64)0x3)
#define MM_PML4E_KERNEL_RO_FLAG ((u64)0x1)

#define MM_PDPTE_KERNEL_GLB_BP_RW_FLAG ((u64)0x183)
#define MM_PDPTE_KERNEL_GLB_RW_FLAG ((u64)0x103)
#define MM_PDPTE_KERNEL_GLB_BP_RO_FLAG ((u64)0x181)
#define MM_PDPTE_KERNEL_GLB_RO_FLAG ((u64)0x101)

#define MM_PDPTE_KERNEL_BP_RW_FLAG ((u64)0x83)
#define MM_PDPTE_KERNEL_RW_FLAG ((u64)0x3)
#define MM_PDPTE_KERNEL_BP_RO_FLAG ((u64)0x81)
#define MM_PDPTE_KERNEL_RO_FLAG ((u64)0x1)

#define MM_PDTE_KERNEL_GLB_BP_RW_FLAG ((u64)0x183)
#define MM_PDTE_KERNEL_RW_FLAG ((u64)0x3)
#define MM_PDTE_KERNEL_GLB_BP_RO_FLAG ((u64)0x181)
#define MM_PDTE_KERNEL_RO_FLAG ((u64)0x1)

#define MM_PDTE_KERNEL_BP_RW_FLAG ((u64)0x83)
#define MM_PDTE_KERNEL_BP_RO_FLAG ((u64)0x81)

#define MM_PTE_KERNEL_RW_FLAG ((u64)0x103)
#define MM_PTE_KERNEL_RO_FLAG ((u64)0x101)

/* 页表大小枚举 */
enum mm_page_size
{
    MM_PAGE_SIZE_4K = 1,
    MM_PAGE_SIZE_2M = 512,
    MM_PAGE_SIZE_1G = 262144,
};

#endif
