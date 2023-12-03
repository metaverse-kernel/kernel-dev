#include <kernel/kernel.h>
#include <kernel/tty.h>

#include <libk/multiboot2.h>

// 通过bootinfo获取临时的帧缓冲区信息
void get_frame_buffer_with_bootinfo(framebuffer *fb, bootinfo_t *bootinfo);

void kmain(void *mb2_bootinfo)
{
    bootinfo_t bootinfo;
    bootinfo_new(&bootinfo, mb2_bootinfo);

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
}
