use core::ptr::null_mut;

use crate::libk::alloc::{
    boxed::Box,
    string::{String, ToString},
    vec::Vec,
};

extern "C" {
    pub fn tty_new(tty_type: u8, mode: u8) -> *mut u8;
    pub fn tty_get(id: usize) -> *mut *mut u8;
    pub fn tty_text_print(ttyx: *mut u8, string: *mut u8, color: u32, bgcolor: u32);
    pub fn tty_get_id(tty: *mut u8) -> usize;

    pub fn tty_get_width(tty: *mut u8) -> usize;
    pub fn tty_get_height(tty: *mut u8) -> usize;

    pub fn tty_get_type(tty: *mut u8) -> u8;
    pub fn tty_get_mode(tty: *mut u8) -> u8;

    pub fn tty_is_enabled(tty: *mut u8) -> bool;

    pub fn tty_enable(tty: *mut u8) -> bool;
    pub fn tty_disable(tty: *mut u8);
}

pub enum Type {
    Invalid = 0,
    RawFramebuffer = 1,
    Display = 2,
    VirtualTerm = 3,
}

impl From<u8> for Type {
    fn from(value: u8) -> Self {
        match value {
            1 => Type::RawFramebuffer,
            2 => Type::Display,
            3 => Type::VirtualTerm,
            _ => Type::Invalid,
        }
    }
}

pub enum Mode {
    Invalid = 0,
    Text = 1,
    Graphics = 2,
}

impl From<u8> for Mode {
    fn from(value: u8) -> Self {
        match value {
            1 => Mode::Text,
            2 => Mode::Graphics,
            _ => Mode::Invalid,
        }
    }
}

pub struct Tty {
    tty_pointer: *mut u8,
}

impl Tty {
    pub fn new(tty_type: Type, tty_mode: Mode) -> Self {
        let tty = unsafe { tty_new(tty_type as u8, tty_mode as u8) };
        Self { tty_pointer: tty }
    }

    pub fn from_id(id: usize) -> Option<Self> {
        let tty = unsafe { tty_get(id) };
        if tty == null_mut() {
            None
        } else {
            let tty = unsafe { *tty };
            Some(Self { tty_pointer: tty })
        }
    }

    pub unsafe fn get_c_tty_t(&self) -> *mut u8 {
        self.tty_pointer
    }

    pub fn id(&self) -> usize {
        unsafe { tty_get_id(self.tty_pointer) }
    }

    pub fn size(&self) -> Resolution {
        unsafe {
            Resolution {
                width: tty_get_width(self.tty_pointer),
                height: tty_get_height(self.tty_pointer),
            }
        }
    }

    pub fn get_type(&self) -> Type {
        let tp = unsafe { tty_get_type(self.tty_pointer) };
        Type::from(tp)
    }

    pub fn mode(&self) -> Mode {
        let mode = unsafe { tty_get_mode(self.tty_pointer) };
        Mode::from(mode)
    }

    pub fn is_enabled(&self) -> bool {
        unsafe { tty_is_enabled(self.tty_pointer) }
    }

    pub fn enable(&self) {
        unsafe { tty_enable(self.tty_pointer) };
    }

    pub fn disable(&self) {
        unsafe { tty_disable(self.tty_pointer) };
    }

    pub fn print(&self, msg: Message) {
        for MessageSection {
            mut msg,
            fgcolor,
            bgcolor,
        } in msg.0.into_iter()
        {
            unsafe {
                let string = msg.as_bytes_mut() as *mut [u8] as *mut u8;
                let string = string.offset(msg.len() as isize);
                let swp = *string;
                *string = 0;
                tty_text_print(
                    self.tty_pointer,
                    msg.as_bytes_mut() as *mut [u8] as *mut u8,
                    u32::from(fgcolor),
                    u32::from(bgcolor),
                );
                *string = swp;
            };
        }
    }
}

#[derive(Clone, Copy, Default)]
pub struct Color(pub u8, pub u8, pub u8);

impl Color {
    pub const WHITE: Color = Color(0xee, 0xee, 0xee);
    pub const BLACK: Color = Color(0, 0, 0);
    pub const RED: Color = Color(0xee, 0x22, 0x22);
    pub const GREEN: Color = Color(0x22, 0xee, 0x22);
    pub const BLUE: Color = Color(0x22, 0x22, 0xee);
    pub const YELLOW: Color = Color(0xee, 0xee, 0x22);
    pub const ORANGE: Color = Color(0xee, 0xaa, 0x22);
    pub const PURPLE: Color = Color(0xee, 0x22, 0xee);
    pub const PINK: Color = Color(0xee, 0x44, 0x66);
    pub const GRAY: Color = Color(0xaa, 0xaa, 0xaa);
}

impl From<Color> for u32 {
    fn from(value: Color) -> Self {
        let res = (value.0 as u32) << 16 | (value.1 as u32) << 8 | (value.2 as u32);
        res
    }
}

pub struct Resolution {
    pub width: usize,
    pub height: usize,
}

#[derive(Clone, Default)]
pub struct MessageSection {
    msg: String,
    fgcolor: Color,
    bgcolor: Color,
}

/// ## Message
///
/// 用`MessageBuilder`构造一个`Message`对象
#[derive(Clone, Default)]
pub struct Message(Vec<MessageSection>);

impl ToString for Message {
    fn to_string(&self) -> String {
        let mut res = String::new();
        for MessageSection { msg, .. } in self.0.iter() {
            res += msg.clone();
        }
        res
    }
}

/// ## MessageBuilder
///
/// 使用链式调用模式构造一个消息.
///
/// 一个`Message`包含多个`MessageSection`，每个`MessageSection`以`message()`调用开始，
/// `message()`调用后可接0个或多个属性设置。
///
/// ```rust
/// MessageBuilder::new()
///     .message("Hello, ")
///     .message("Metaverse").foreground_color(Color(0xa, 0xee, 0xa))
///     .message("!\n")
///     .build();
/// ```
///
/// 定义了`message!`宏，简化构造消息的代码：
///
/// ```rust
/// use crate::kernel::tty::tty::BuilderFunctions::*;
///
/// message_raw!(
///     Msg("Hello, "),
///     Msg("Metaverse"),
///     FgColor(Color::GREEN),
///     Msg("!\n"),
/// );
/// ```
///
/// 对于特殊情况可以使用非链式调用：
/// ```rust
/// let mut msg = MessageBuilder::new();
/// msg.message_mut("Hello, ");
/// msg.message_mut("Metaverse");
/// msg.foreground_color(Color(0xa, 0xee, 0xa));
/// msg.message_mut("!\n");
/// let msg = msg.build();
/// ```
pub struct MessageBuilder {
    msg: Message,
}

pub enum BuilderFunctions<T: ToString> {
    Msg(T),
    FgColor(Color),
    BgColor(Color),
}

impl MessageBuilder {
    pub fn new() -> Self {
        Self {
            msg: Message(Vec::new()),
        }
    }

    pub fn is_empty(&self) -> bool {
        self.msg.0.is_empty()
    }

    pub fn from_message(msg: Message) -> Self {
        Self { msg }
    }

    pub fn message<T: ToString + ?Sized>(mut self, msg: &T) -> Self {
        self.msg.0.push(MessageSection {
            msg: msg.to_string(),
            fgcolor: Color(0xee, 0xee, 0xee),
            bgcolor: Color(0, 0, 0),
        });
        self
    }

    pub fn message_mut<T: ToString + ?Sized>(&mut self, msg: &T) {
        self.msg.0.push(MessageSection {
            msg: msg.to_string(),
            fgcolor: Color(0xee, 0xee, 0xee),
            bgcolor: Color(0, 0, 0),
        });
    }

    pub fn background_color(mut self, color: Color) -> Self {
        if let Some(msg) = self.msg.0.last_mut() {
            msg.bgcolor = color;
        }
        self
    }

    pub fn background_color_mut(&mut self, color: Color) {
        if let Some(msg) = self.msg.0.last_mut() {
            msg.bgcolor = color;
        }
    }

    pub fn foreground_color(mut self, color: Color) -> Self {
        if let Some(msg) = self.msg.0.last_mut() {
            msg.fgcolor = color;
        }
        self
    }

    pub fn foreground_color_mut(&mut self, color: Color) {
        if let Some(msg) = self.msg.0.last_mut() {
            msg.fgcolor = color;
        }
    }

    pub fn append(mut self, mut builder: Self) -> Self {
        self.msg.0.append(&mut builder.msg.0);
        self
    }

    pub fn build(self) -> Message {
        self.msg
    }
}

pub enum FmtMeta {
    Color(Color),
    String(String),
    ToStringable(Box<dyn ToString>),
    Hex(u8),
    Pointer(usize),
}

pub fn format_message(fmt: &mut Vec<char>, meta: FmtMeta) -> MessageBuilder {
    let mut msgbuilder = MessageBuilder::new();
    let mut fmt_start = None;
    let mut fmt_end = None;
    for i in 0..fmt.len() {
        if fmt[i] == '{' {
            fmt_start = Some(i);
        }
        if fmt[i] == '}' {
            fmt_end = Some(i);
            break;
        }
    }
    let tohex = |n: u8| {
        if n < 10 {
            (b'0' + n) as char
        } else {
            (b'a' + n - 10) as char
        }
    };
    if fmt_start == None || fmt_end == None {
        msgbuilder.message_mut(&String::from_iter(fmt.iter()));
    } else {
        let fmt_start = fmt_start.unwrap();
        let fmt_end = fmt_end.unwrap();
        let mut formatter = Vec::new();
        for _ in fmt_start..=fmt_end {
            formatter.push(fmt.remove(fmt_start));
        }
        formatter.remove(formatter.len() - 1);
        formatter.remove(0);
        match meta {
            FmtMeta::Color(color) => {
                let first = fmt.split_at(fmt_start).0;
                msgbuilder.message_mut(&String::from_iter(first.iter()));
                for _ in 0..fmt_start {
                    fmt.remove(0);
                }
                msgbuilder.message_mut(&String::from_iter(formatter.iter()));
                msgbuilder.foreground_color_mut(color);
            }
            FmtMeta::String(s) => {
                for c in s.chars().rev() {
                    fmt.insert(fmt_start, *c);
                }
            }
            FmtMeta::ToStringable(s) => {
                for c in s.to_string().chars().rev() {
                    fmt.insert(fmt_start, *c);
                }
            }
            FmtMeta::Hex(num) => {
                fmt.insert(fmt_start, tohex(num >> 4));
                fmt.insert(fmt_start, tohex(num & 0xf));
            }
            FmtMeta::Pointer(mut p) => {
                for _ in 0..16 {
                    fmt.insert(fmt_start, tohex((p & 0xf) as u8));
                    p >>= 4;
                }
            }
        }
    }
    let mut rests = Vec::new();
    while !fmt.is_empty() && fmt[0] != '{' {
        rests.push(fmt.remove(0));
    }
    msgbuilder.message_mut(&String::from_iter(rests.iter()));
    msgbuilder
}
