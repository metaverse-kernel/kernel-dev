#include <libk/multiboot2.h>
#include <libk/string.h>

typedef struct __bootinfo_next_result_t
{
    u32 type;
    void *addr;
} bootinfo_next_result_t;

void bootinfo_next(bootinfo_t *bootinfo, bootinfo_next_result_t *result)
{
    if (bootinfo->iter_index >= bootinfo->size)
    {
        result->type = 0;
        result->addr = nullptr;
    }
    bootinfo_tag_t *tag = bootinfo->start + bootinfo->iter_index;
    result->type = tag->type;
    result->addr = bootinfo->start + bootinfo->iter_index + sizeof(bootinfo_tag_t);
    bootinfo->iter_index += tag->size;
    if (bootinfo->iter_index % 8)
    {
        bootinfo->iter_index = (bootinfo->iter_index / 8 + 1) * 8;
    }
}

void bootinfo_new(bootinfo_t *bootinfo, void *bootinfo_addr)
{
    bootinfo->start = bootinfo_addr;
    bootinfo->size = *((u32 *)bootinfo_addr);
    bootinfo->iter_index = 8; // 跳过bootinfo头
    bootinfo_next_result_t res;
    memset(bootinfo->map, 0, sizeof(bootinfo->map));
    memset(bootinfo->map_counts, 0, sizeof(bootinfo->map_counts));
    while ((bootinfo_next(bootinfo, &res), res.type))
    {
        if (res.type == BOOTINFO_EFI_BOOTSERVICE_NOT_TERMINATED_TYPE)
        {
            bootinfo->map_counts[res.type] = true;
        }
        else if (res.addr && bootinfo->map_counts[res.type] < 16)
        { // 同类型tag数量只能16个以内
            bootinfo->map[res.type][bootinfo->map_counts[res.type]++] = res.addr;
        }
    }
}

usize bootinfo_get_tag(bootinfo_t *bootinfo, usize type, void ***tags)
{
    *tags = bootinfo->map[type];
    return bootinfo->map_counts[type];
}
