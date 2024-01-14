#include <kernel/kernel.h>
#include <kernel/tty.h>
#include <kernel/memm.h>

#include <libk/multiboot2.h>
#include <libk/math.h>

// 通过bootinfo获取临时的帧缓冲区信息
void get_frame_buffer_with_bootinfo(framebuffer *fb, bootinfo_t *bootinfo);

void kmain(void *mb2_bootinfo)
{
    // 创建bootinfo对象
    bootinfo_t bootinfo;
    bootinfo_new(&bootinfo, mb2_bootinfo);

    // 获取内存信息
    void **tags;
    usize tags_amount;
    if ((tags_amount = bootinfo_get_tag(&bootinfo, BOOTINFO_MEMORY_MAP_TYPE, &tags)) == 0)
    {
        KERNEL_TODO();
    }
    bootinfo_memory_map_t *meminfo = bootinfo_memory_map(tags[0]);
    usize mem_size = 0;
    for (
        bootinfo_memory_map_entry_t *it = meminfo->entries;
        (void *)it < bootinfo_memory_map_end(meminfo);
        it++)
    {
        if (it->type == 1 || it->type == 3 || it->type == 4 || it->type == 5)
            mem_size += it->length;
    }

    // 初始化内存管理模块
    mem_manager_t *memm = memm_new(mem_size);

    // 初始化tty模块
    tty_controller_t *tty_controler = tty_controller_new();

    framebuffer fb;
    get_frame_buffer_with_bootinfo(&fb, &bootinfo);
    tty *tty0 = tty_new(tty_type_raw_framebuffer, tty_mode_text);
    tty_set_framebuffer(tty0, &fb);

    tty_text_print(tty0, "Hello Metaverse!\n", gen_color(0xee, 0xee, 0xee), gen_color(0, 0, 0));

    // 初始化内核日志模块

    while (true)
    {
    }
}

void get_frame_buffer_with_bootinfo(framebuffer *fb, bootinfo_t *bootinfo)
{
    void **tags;
    usize tags_amount;
    if ((tags_amount = bootinfo_get_tag(bootinfo, BOOTINFO_FRAMEBUFFER_INFO_TYPE, &tags)) == 0)
    {
        KERNEL_TODO();
    }
    bootinfo_framebuffer_info_t *fbinfo = bootinfo_framebuffer_info(tags[0]);
    fb->pointer = (void *)fbinfo->framebuffer_addr;
    fb->width = fbinfo->framebuffer_width;
    fb->height = fbinfo->framebuffer_height;
    fb->pixsize = fbinfo->framebuffer_bpp / 8;
    fb->pixtype = rgb;
    memm_map_pageframes_to(
        (u64)fb->pointer, (u64)fb->pointer,
        fb->width * fb->height * fb->pixsize,
        false, true);
}
