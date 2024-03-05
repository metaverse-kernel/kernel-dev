use core::ptr::null_mut;

use alloc::{
    string::{String, ToString},
    vec,
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

#[derive(Clone, Copy)]
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

#[derive(Clone)]
pub struct MessageSection {
    msg: String,
    fgcolor: Color,
    bgcolor: Color,
}

/// ## Message
///
/// 用`MessageBuilder`构造一个`Message`对象
#[derive(Clone)]
pub struct Message(Vec<MessageSection>);

impl ToString for Message {
    fn to_string(&self) -> String {
        let mut res = String::new();
        for MessageSection { msg, .. } in self.0.iter() {
            res += msg;
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
/// message!(
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

pub enum BuilderFunctions<'a> {
    Msg(&'a str),
    FgColor(Color),
    BgColor(Color),
}

impl MessageBuilder {
    pub fn new() -> Self {
        Self {
            msg: Message(vec![]),
        }
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
