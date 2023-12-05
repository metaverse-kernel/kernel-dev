#include <kernel/kernel.h>
#include <kernel/tty.h>
#include <kernel/memm.h>

#include <libk/multiboot2.h>

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
    if ((tags_amount = bootinfo_get_tag(bootinfo, BOOTINFO_BASIC_MEMORY_INFO_TYPE, &tags)) == 0)
    {
        KERNEL_TODO();
    }
    bootinfo_basic_memory_info_t *meminfo = bootinfo_basic_memory_info(tags[0]);
    usize mem_size = 1024 * 1024 + meminfo->mem_upper * 1024;

    // 初始化内存管理模块
    mem_manager_t *memm = memm_new(mem_size);

    tty_controller_init();

    framebuffer fb;
    get_frame_buffer_with_bootinfo(&fb, &bootinfo);
    tty tty0;
    tty_new(&tty0, tty_type_raw_framebuffer, tty_mode_text);
    tty_set_framebuffer(&tty0, &fb);

    tty_text_print(&tty0, "Hello Metaverse!\n", gen_color(0xbb, 0xbb, 0xbb), gen_color(0, 0, 0));

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
    // TODO 对应内存空间还没有分页
    KERNEL_TODO();
    for (usize i = 0; i < 10000; i++)
    {
        ((u8 *)fb->pointer)[i] = 0xff;
    }
    KERNEL_TODO();
}
