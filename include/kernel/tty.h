#ifndef TTY_H
#define TTY_H

#include <types.h>
#include <kernel/memm.h>

typedef enum __tty_type
{
    invalid,
    // 用于在内核刚刚被引导，只有bootloader提供的显示功能时使用
    tty_type_raw_framebuffer,
    // 用于图形功能初始化后，直接连接图形接口
    tty_type_display,
    // 用于图形终端的终端模拟器
    tty_type_vtty,
} tty_type;

typedef enum __framebuffer_pixel_type
{
    rgb,
    bgr,
} framebuffer_pixel_type;

// 在不同的tty类型下，提供相应的使用显示功能的信息
typedef union __tty_typeinfo
{
    struct __framebuffer
    {
        void *pointer;
        usize width, height;
        framebuffer_pixel_type pixtype;
        u8 pixsize;
    } raw_framebuffer;
} tty_typeinfo;

typedef struct __framebuffer framebuffer;

// tty模式
// 分为文本模式和图形模式
// 文本模式中的字符由tty模块渲染
typedef enum __tty_mode
{
    tty_mode_text,
    tty_mode_graphics,
} tty_mode;

typedef struct __tty_text_state
{
    usize line, column;
    usize width, height;
    volatile u8 lock;
} tty_text_state;

// tty对象
typedef struct __tty
{
    usize id;
    tty_type type;
    tty_typeinfo typeinfo;
    tty_mode mode;
    tty_text_state text;
} tty;

// tty控制器
typedef struct __tty_controller_t
{
#define TTY_MAX_NUM 128
    tty *ttys[TTY_MAX_NUM];
    bool map[TTY_MAX_NUM];
} tty_controller_t;

/**
 * @brief 初始化tty控制器
 *
 */
tty_controller_t *tty_controller_new();

/**
 * @brief 创建tty
 *
 * @param __tty 若为nullptr，将使用kmalloc申请内存，需要手动释放
 * @param type
 * @return tty* 返回tty对象的地址，如果tty数量超过TTY_MAX_NUM返回nullptr，
 *          无论传入的__tty是否是nullptr
 */
tty *tty_new(tty_type type, tty_mode mode);

/**
 * @brief 通过tty id获取一个tty
 *
 * @param id
 * @return tty** 二级指针方便判断这个tty是否删除，返回空指针表示当前没有这个tty
 */
tty **tty_get(usize id);

/**
 * @brief 当type为raw_framebuffer时设置帧缓冲区
 *
 * @param ttyx
 * @param fb
 */
void tty_set_framebuffer(tty *ttyx, framebuffer *fb);

/**
 * @brief 当mode为text时打印ascii文字
 *
 * @param ttyx
 * @param string
 */
void tty_text_print(tty *ttyx, char *string, u32 color, u32 bgcolor);

#define gen_color(r, g, b) (((r) << 16) | ((g) << 8) | (b))

#define TTY_FONT_SCALE 2

typedef struct __tty_font_t
{
    u16 char_width, char_height;
    u64 **font;
} tty_font_t;

tty_font_t *tty_get_font();

#endif
