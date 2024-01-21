use std::ptr::null_mut;

extern "C" {
    fn tty_new(tty_type: u8, mode: u8) -> *mut u8;
    fn tty_get(id: usize) -> *mut *mut u8;
    pub fn tty_text_print(ttyx: *mut u8, string: *mut u8, color: u32, bgcolor: u32);
    fn tty_get_id(tty: *mut u8) -> usize;

    fn tty_enable(tty: *mut u8) -> bool;
    fn tty_disable(tty: *mut u8);
}

pub enum Type {
    Invalid = 0,
    RawFramebuffer = 1,
    Display = 2,
    VirtualTerm = 3,
}

pub enum Mode {
    Text = 0,
    Graphics = 1,
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

pub struct Color(pub u8, pub u8, pub u8);

impl From<Color> for u32 {
    fn from(value: Color) -> Self {
        let res = (value.0 as u32) << 16 | (value.1 as u32) << 8 | (value.2 as u32);
        res
    }
}

pub struct MessageSection {
    msg: String,
    fgcolor: Color,
    bgcolor: Color,
}

pub struct Message(Vec<MessageSection>);

pub struct MessageBuilder {
    msg: Message,
}

impl MessageBuilder {
    pub fn new() -> Self {
        Self {
            msg: Message(vec![]),
        }
    }

    pub fn message(mut self, msg: String) -> Self {
        self.msg.0.push(MessageSection {
            msg,
            fgcolor: Color(0xee, 0xee, 0xee),
            bgcolor: Color(0, 0, 0),
        });
        self
    }

    pub fn background_color(mut self, color: Color) -> Self {
        if let Some(msg) = self.msg.0.last_mut() {
            msg.bgcolor = color;
        }
        self
    }

    pub fn foreground_color(mut self, color: Color) -> Self {
        if let Some(msg) = self.msg.0.last_mut() {
            msg.fgcolor = color;
        }
        self
    }

    pub fn build(self) -> Message {
        self.msg
    }
}
