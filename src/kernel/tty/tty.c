#include <kernel/tty.h>

#include <kernel/kernel.h>

#include <libk/string.h>
#include <libk/string.h>

tty_controller_t tty_ctrler;

tty_controller_t *tty_controller_new()
{
    memset(tty_ctrler.ttys, 0, sizeof(tty_ctrler.ttys));
    memset(tty_ctrler.map, 0, sizeof(tty_ctrler.map));
    memset(tty_ctrler.enabled, 0, sizeof(tty_ctrler.enabled));
    return &tty_ctrler;
}

tty *tty_new(tty_type type, tty_mode mode)
{
    tty *res = nullptr;
    for (usize i = 0; i < TTY_MAX_NUM; ++i)
    {
        if (tty_ctrler.map[i] == false)
        {
            res = memm_kernel_allocate(sizeof(tty));
            res->id = i;
            tty_ctrler.ttys[i] = res;
            tty_ctrler.map[i] = true;
            break;
        }
    }
    res->type = type;
    res->mode = mode;
    res->enabled = false;
    res->width = 0;
    res->height = 0;
    return res;
}

tty **tty_get(usize id)
{
    if (tty_ctrler.map[id] == false)
        return nullptr;
    return &tty_ctrler.ttys[id];
}

usize tty_get_id(tty *__tty)
{
    return __tty->id;
}

void tty_set_framebuffer(tty *ttyx, framebuffer *fb)
{
    if (ttyx->type != tty_type_raw_framebuffer)
        return;
    memcpy(&ttyx->typeinfo.raw_framebuffer, fb, sizeof(framebuffer));
    ttyx->width = ttyx->typeinfo.raw_framebuffer.width;
    ttyx->height = ttyx->typeinfo.raw_framebuffer.height;
    if (ttyx->mode == tty_mode_text)
    {
        ttyx->text.width = fb->width / (TTY_FONT_SCALE * tty_get_font()->char_width);
        ttyx->text.height = fb->height / (TTY_FONT_SCALE * tty_get_font()->char_height);
        ttyx->text.line = 0;
        ttyx->text.column = 0;
    }
}

inline static void put_pixel(
    void *buffer, usize width, usize pixsize,
    usize x, usize y, u32 pix)
{
    *(u32 *)(buffer + y * width * pixsize + x * pixsize) = pix;
}

inline static void scroll_buffer(
    void *buffer, usize width, usize height, usize pixsize,
    usize dist)
{
    memcpy(buffer, buffer + width * pixsize * dist, (height - dist) * width * pixsize);
    memset(buffer + (height - dist) * width * pixsize, 0, dist * width * pixsize);
}

inline static void putchar(
    tty *ttyx, char c, u32 color, u32 bgcolor)
{
    tty_font_t *font = tty_get_font();
    for (usize j = 0; j < font->char_height; ++j)
    {
        for (usize i = 0; i < font->char_width; ++i)
        {
            bool p = font->font[c][j] & (1 << (font->char_width - 1 - i));
            if (p != false)
            {
                for (usize a = 0; a < TTY_FONT_SCALE; ++a)
                {
                    for (usize b = 0; b < TTY_FONT_SCALE; ++b)
                    {
                        put_pixel(
                            ttyx->typeinfo.raw_framebuffer.pointer,
                            ttyx->typeinfo.raw_framebuffer.width,
                            ttyx->typeinfo.raw_framebuffer.pixsize,
                            ttyx->text.column * font->char_width * TTY_FONT_SCALE + i * TTY_FONT_SCALE + a,
                            ttyx->text.line * font->char_height * TTY_FONT_SCALE + j * TTY_FONT_SCALE + b,
                            color);
                    }
                }
            }
            else
            {
                for (usize a = 0; a < TTY_FONT_SCALE; ++a)
                {
                    for (usize b = 0; b < TTY_FONT_SCALE; ++b)
                    {
                        put_pixel(
                            ttyx->typeinfo.raw_framebuffer.pointer,
                            ttyx->typeinfo.raw_framebuffer.width,
                            ttyx->typeinfo.raw_framebuffer.pixsize,
                            ttyx->text.column * font->char_width * TTY_FONT_SCALE + i * TTY_FONT_SCALE + a,
                            ttyx->text.line * font->char_height * TTY_FONT_SCALE + j * TTY_FONT_SCALE + b,
                            bgcolor);
                    }
                }
            }
        }
    }
}

inline static void newline(tty *ttyx)
{
    ttyx->text.column = 0;
    ttyx->text.line++;
    if (ttyx->text.line == ttyx->text.height)
    {
        scroll_buffer(
            ttyx->typeinfo.raw_framebuffer.pointer,
            ttyx->typeinfo.raw_framebuffer.width,
            ttyx->typeinfo.raw_framebuffer.height,
            ttyx->typeinfo.raw_framebuffer.pixsize,
            tty_get_font()->char_height * TTY_FONT_SCALE);
        ttyx->text.line--;
    }
}

void tty_text_print(tty *ttyx, char *string, u32 color, u32 bgcolor)
{
    if (ttyx->enabled == false)
        return;
    // TODO 暂时只支持framebuffer
    if (ttyx->type != tty_type_raw_framebuffer)
        return;
    if (ttyx->mode != tty_mode_text)
        return;
    if (ttyx->typeinfo.raw_framebuffer.pixtype == bgr)
    { // bgr格式要把三个颜色的顺序倒过来
        u32 t = color;
        color = 0;
        i32 n = 3;
        while (n-- != 0)
        {
            color <<= 8;
            color += t & 0xff;
            t >>= 8;
        }
    }
    tty_font_t *font = tty_get_font();
    usize len = strlen(string);
    simple_lock_lock(ttyx->text.lock);
    for (char *str = string; string - str < len; string++)
    {
        char c = *string;
        if (c == '\n')
        { // 换行
            putchar(ttyx, ' ', 0, 0);
            newline(ttyx);
            continue;
        }
        else if (c == '\t')
        { // 水平制表符
            putchar(ttyx, ' ', 0, 0);
            ttyx->text.column += 8;
            ttyx->text.column -= ttyx->text.column % 8;
            continue;
        }
        else if (c == '\r')
        { // 回到行首
            putchar(ttyx, ' ', 0, 0);
            ttyx->text.column = 0;
            continue;
        }
        else if (c == '\v')
        { // 垂直制表符
            putchar(ttyx, ' ', 0, 0);
            ttyx->text.line++;
            if (ttyx->text.line == ttyx->text.height)
            {
                scroll_buffer(
                    ttyx->typeinfo.raw_framebuffer.pointer,
                    ttyx->typeinfo.raw_framebuffer.width,
                    ttyx->typeinfo.raw_framebuffer.height,
                    ttyx->typeinfo.raw_framebuffer.pixsize,
                    font->char_height * TTY_FONT_SCALE);
                ttyx->text.line--;
            }
            continue;
        }
        else if (c == '\b')
        { // 退格
            putchar(ttyx, ' ', 0, 0);
            ttyx->text.column--;
            if (ttyx->text.column < 0)
                ttyx->text.column = 0;
            continue;
        }
        else if (c == '\f')
        { // 滚动一行
            scroll_buffer(
                ttyx->typeinfo.raw_framebuffer.pointer,
                ttyx->typeinfo.raw_framebuffer.width,
                ttyx->typeinfo.raw_framebuffer.height,
                ttyx->typeinfo.raw_framebuffer.pixsize,
                font->char_height * TTY_FONT_SCALE);
            continue;
        }
        // 打印字符c
        if (*(string + 1) != '\0' && ttyx->text.column + 1 == ttyx->text.width)
        {
            putchar(ttyx, '\n', color / 4 * 3, bgcolor / 4 * 3);
            newline(ttyx);
        }
        putchar(ttyx, c, color, bgcolor);
        ttyx->text.column++;
    }
    if (ttyx->text.column == ttyx->text.width)
        newline(ttyx);
    putchar(ttyx, '\0', gen_color(0x88, 0x88, 0x88), 0);
    simple_lock_unlock(ttyx->text.lock);
}

usize tty_get_width(tty *ttyx)
{
    if (ttyx->mode == tty_mode_text)
        return ttyx->text.width;
    return ttyx->width;
}

usize tty_get_height(tty *ttyx)
{
    if (ttyx->mode == tty_mode_text)
        return ttyx->text.height;
    return ttyx->height;
}

tty_type tty_get_type(tty *ttyx)
{
    return ttyx->type;
}

tty_mode tty_get_mode(tty *ttyx)
{
    return ttyx->mode;
}

bool tty_is_enabled(tty *ttyx)
{
    return ttyx->enabled;
}

bool tty_enable(tty *ttyx)
{
    if (tty_ctrler.enabled[ttyx->id])
        return false;
    if (ttyx->type == tty_type_raw_framebuffer)
    {
        for (usize i = 0; i < TTY_MAX_NUM; ++i)
        {
            if (ttyx->id != i &&
                tty_ctrler.enabled[i] != nullptr &&
                tty_ctrler.ttys[i]->type == tty_type_raw_framebuffer)
                return false;
        }
    }

    ttyx->enabled = true;
    tty_ctrler.enabled[ttyx->id] = ttyx;
    return true;
}

void tty_disable(tty *ttyx)
{
    ttyx->enabled = false;
    tty_ctrler.enabled[ttyx->id] = false;
}
