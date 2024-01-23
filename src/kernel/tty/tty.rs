use core::ptr::null_mut;

use alloc::{
    format,
    string::{String, ToString},
    vec,
    vec::Vec,
};

extern "C" {
    fn tty_new(tty_type: u8, mode: u8) -> *mut u8;
    fn tty_get(id: usize) -> *mut *mut u8;
    fn tty_text_print(ttyx: *mut u8, string: *mut u8, color: u32, bgcolor: u32);
    fn tty_get_id(tty: *mut u8) -> usize;

    fn tty_get_width(tty: *mut u8) -> usize;
    fn tty_get_height(tty: *mut u8) -> usize;

    fn tty_get_type(tty: *mut u8) -> u8;
    fn tty_get_mode(tty: *mut u8) -> u8;

    fn tty_is_enabled(tty: *mut u8) -> bool;

    fn tty_enable(tty: *mut u8) -> bool;
    fn tty_disable(tty: *mut u8);
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

impl ToString for Color {
    fn to_string(&self) -> String {
        format!("{:02x}{:02x}{:02x}", self.0, self.1, self.2)
    }
}

pub enum ColorPair {
    Reset,
    Color { fore: Color, back: Color },
}

impl ColorPair {
    pub fn reset() -> Self {
        ColorPair::Reset
    }

    pub fn color(fore: Color, back: Color) -> Self {
        ColorPair::Color { fore, back }
    }
}

impl ToString for ColorPair {
    fn to_string(&self) -> String {
        match self {
            ColorPair::Reset => format!("\x1b{{}}"),
            ColorPair::Color { fore, back } => {
                format!("\x1b{{{}{}}}", fore.to_string(), back.to_string())
            }
        }
    }
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

#[derive(Clone)]
pub struct Message(Vec<MessageSection>);

impl From<String> for Message {
    fn from(value: String) -> Self {
        let mut res = MessageBuilder::new();
        let mut msg = String::new();
        let mut color = ColorPair::Reset;
        let mut coloring = false;

        let mut colorp_str = String::new();

        for c in value.as_bytes() {
            if *c as char == '\x1b' {
                coloring = true;
                res.message_mut(msg.clone());
                msg.clear();
                if let ColorPair::Color { fore, back } = color {
                    res.foreground_color_mut(fore);
                    res.background_color_mut(back);
                }
                continue;
            }
            if coloring {
                if *c as char == '{' {
                    colorp_str.clear();
                } else if *c as char == '}' {
                    if colorp_str.is_empty() {
                        color = ColorPair::Reset;
                    } else {
                        let bts = colorp_str.as_bytes();
                        let r = bts[0] << 4 + bts[1];
                        let g = bts[2] << 4 + bts[3];
                        let b = bts[4] << 4 + bts[5];
                        let fore = Color(r, g, b);
                        let r = bts[6] << 4 + bts[7];
                        let g = bts[8] << 4 + bts[9];
                        let b = bts[10] << 4 + bts[11];
                        let back = Color(r, g, b);
                        color = ColorPair::Color { fore, back };
                    }
                    coloring = false;
                } else {
                    colorp_str.push(*c as char);
                }
                continue;
            }
            msg.push(*c as char);
        }
        if !msg.is_empty() {
            res.message_mut(msg.clone());
            msg.clear();
            if let ColorPair::Color { fore, back } = color {
                res.foreground_color_mut(fore);
                res.background_color_mut(back);
            }
        }
        res.build()
    }
}

pub struct MessageBuilder {
    msg: Message,
}

impl MessageBuilder {
    pub fn new() -> Self {
        Self {
            msg: Message(vec![]),
        }
    }

    pub fn from_message(msg: &Message) -> Self {
        Self { msg: msg.clone() }
    }

    pub fn message(mut self, msg: String) -> Self {
        self.msg.0.push(MessageSection {
            msg,
            fgcolor: Color(0xee, 0xee, 0xee),
            bgcolor: Color(0, 0, 0),
        });
        self
    }

    pub fn message_mut(&mut self, msg: String) {
        self.msg.0.push(MessageSection {
            msg,
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
