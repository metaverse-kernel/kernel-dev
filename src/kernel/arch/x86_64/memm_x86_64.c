#include <kernel/arch/x86_64/memm.h>

#include <kernel/memm.h>

#include <libk/string.h>
#include <libk/math.h>

#define map_pagemap(addr) \
    map_pageframe_to((u64)addr, (u64)addr, false, true, MEMM_PAGE_SIZE_4K);

// 这里的physical必须保证根据ps对齐
static void map_pageframe_to(u64 target, u64 physical,
                             bool user, bool write, memm_page_size ps)
{
    if (!is_cannonical(target))
        return;

    usize pml4ei = memm_la_get_entry_index(target, MEMM_LA_PML4EI);
    usize pml4e = PML4[pml4ei];
    u64 *PDPT;
    if (memm_entry_flag_get(pml4e, MEMM_ENTRY_FLAG_PRESENT) == true)
        PDPT = (u64 *)memm_entry_get_address(pml4e);
    else
    {
        PDPT = (u64 *)(find_fitable_pages(1) * MEMM_PAGE_SIZE);
        map_pagemap(PDPT);
        memset(PDPT, 0, MEMM_PAGE_SIZE);

        PML4[pml4ei] =
            MEMM_ENTRY_FLAG_PRESENT |
            MEMM_ENTRY_FLAG_WRITE |
            (u64)PDPT;
    }

    usize pdptei = memm_la_get_entry_index(target, MEMM_LA_PDPTEI);
    if (ps == MEMM_PAGE_SIZE_1G)
    {
        PDPT[pdptei] =
            MEMM_ENTRY_FLAG_PRESENT |
            (write ? MEMM_ENTRY_FLAG_WRITE : 0) |
            (is_user_address(target) ? MEMM_ENTRY_FLAG_USER : 0) |
            MEMM_ENTRY_FLAG_PS |
            (is_user_address(target) ? 0 : MEMM_ENTRY_FLAG_GLOBAL) |
            physical;
        return;
    }
    usize pdpte = PDPT[pdptei];
    u64 *PDT;
    if (memm_entry_flag_get(pdpte, MEMM_ENTRY_FLAG_PRESENT) == true)
        PDT = (u64 *)memm_entry_get_address(pdpte);
    else
    {
        PDT = (u64 *)(find_fitable_pages(1) * MEMM_PAGE_SIZE);
        map_pagemap(PDT);
        memset(PDT, 0, MEMM_PAGE_SIZE);

        PDPT[pdptei] =
            MEMM_ENTRY_FLAG_PRESENT |
            MEMM_ENTRY_FLAG_WRITE |
            (u64)PDT;
    }

    usize pdei = memm_la_get_entry_index(target, MEMM_LA_PDEI);
    if (ps == MEMM_PAGE_SIZE_2M)
    {
        PDT[pdei] =
            MEMM_ENTRY_FLAG_PRESENT |
            (write ? MEMM_ENTRY_FLAG_WRITE : 0) |
            (is_user_address(target) ? MEMM_ENTRY_FLAG_USER : 0) |
            MEMM_ENTRY_FLAG_PS |
            (is_user_address(target) ? 0 : MEMM_ENTRY_FLAG_GLOBAL) |
            physical;
        return;
    }
    usize pde = PDT[pdei];
    u64 *PT;
    if (memm_entry_flag_get(pde, MEMM_ENTRY_FLAG_PRESENT) == true)
        PT = (u64 *)memm_entry_get_address(pde);
    else
    {
        PT = (u64 *)(find_fitable_pages(1) * MEMM_PAGE_SIZE);
        map_pagemap(PT);
        memset(PT, 0, MEMM_PAGE_SIZE);

        PDT[pdei] =
            MEMM_ENTRY_FLAG_PRESENT |
            MEMM_ENTRY_FLAG_WRITE |
            (u64)PT;
    }

    usize pei = memm_la_get_entry_index(target, MEMM_LA_PEI);
    PT[pei] =
        MEMM_ENTRY_FLAG_PRESENT |
        (write ? MEMM_ENTRY_FLAG_WRITE : 0) |
        (is_user_address(target) ? MEMM_ENTRY_FLAG_USER : 0) |
        MEMM_ENTRY_FLAG_PS |
        (is_user_address(target) ? 0 : MEMM_ENTRY_FLAG_GLOBAL) |
        physical;
    return;
}

bool memm_map_pageframes_to(
    u64 target, u64 physical,
    usize size,
    bool user, bool write)
{
    if (!is_cannonical(target) || !is_cannonical(physical))
        return false;
    if (!is_aligned(target, MEMM_PAGE_SIZE) || !is_aligned(physical, MEMM_PAGE_SIZE))
        return false;
    while (size != 0)
    {
        memm_page_size align = memm_get_page_align(physical);
        if (align == MEMM_PAGE_SIZE_1G)
        {
            if (size < (usize)align * MEMM_PAGE_SIZE / 2)
                align = MEMM_2M_ALIGN_MASK + 1;
        }
        if (align == MEMM_PAGE_SIZE_2M)
        {
            if (size < (usize)align * MEMM_PAGE_SIZE / 2)
                align = MEMM_4K_ALIGN_MASK + 1;
        }
        align /= MEMM_PAGE_SIZE;
        memory_manager_t *mm = memm_get_manager();
        switch (align)
        {
        case MEMM_PAGE_SIZE_4K:
            mm->platformed_page_counter.mapped_4k_page++;
        case MEMM_PAGE_SIZE_2M:
            mm->platformed_page_counter.mapped_2m_page++;
        case MEMM_PAGE_SIZE_1G:
            mm->platformed_page_counter.mapped_1g_page++;
        }
        for (usize i = physical / MEMM_PAGE_SIZE; i < physical / MEMM_PAGE_SIZE + align; i++)
        {
            bitmap_set(mm->page_map, i);
        }

        map_pageframe_to(target, physical, user, write, align);

        usize step = min(size, (usize)align * MEMM_PAGE_SIZE);
        size -= step;
        target += step;
        physical += step;
    }
    reload_pml4();
    return true;
}
