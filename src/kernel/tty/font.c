#include <types.h>

#include <kernel/tty.h>
#include <kernel/memm.h>
#include <libk/string.h>

extern void _binary_font_file_tfo_start;

u64 *font_pointers[256];
tty_font_t refont = {
    .initialized = false,
    .font = (u64 **)font_pointers,
    .char_width = 8,
    .char_height = 16,
};

tty_font_t *tty_get_font()
{
    if (refont.initialized == false)
    {
        void *buffer = &_binary_font_file_tfo_start;
        refont.char_width = ((u64 *)buffer)[0];
        refont.char_height = ((u64 *)buffer)[1];
        void *font = buffer + sizeof(u64) * 64;
        for (usize i = 0; i < 256; i++)
        {
            font_pointers[i] = font + i * sizeof(u64) * 64;
        }
        refont.initialized = true;
    }
    return &refont;
}
