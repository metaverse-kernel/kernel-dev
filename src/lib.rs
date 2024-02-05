#![no_std]

extern crate alloc;

use alloc::{
    string::{String, ToString},
    vec::Vec,
};
use core::{panic::PanicInfo, ptr::null_mut};

use kernel::tty::tty::{self, tty_enable, tty_text_print, Color};

pub mod kernel;
pub mod libk;

#[panic_handler]
unsafe fn kernel_panic_handler(info: &PanicInfo) -> ! {
    let line_in_file = if let Some(loca) = info.location() {
        loca.line().to_string()
    } else {
        String::new()
    };
    let info = {
        let mut v = Vec::new();
        v.push(("Kernel Panic: ", (Color::RED, Color::BLACK)));
        v.push((
            if let Some(loca) = info.location() {
                loca.file()
            } else {
                "NoFile"
            },
            (Color::GREEN, Color::BLACK),
        ));
        v.push((":", (Color::WHITE, Color::BLACK)));
        v.push((
            if let Some(_) = info.location() {
                line_in_file.as_str()
            } else {
                "NoLine"
            },
            (Color::WHITE, Color::BLACK),
        ));
        v.push((": ", (Color::WHITE, Color::BLACK)));
        v.push((
            if let Some(&s) = info.payload().downcast_ref::<&str>() {
                s
            } else {
                "Unknown panic."
            },
            (Color::BLUE, Color::BLACK),
        ));
        v.push(("\n", (Color::BLACK, Color::BLACK)));
        v
    };
    let tty = tty::tty_get(0);
    if tty != null_mut() {
        let tty = *tty;
        tty_enable(tty);
        for (msgo, (fgc, bgc)) in info.into_iter() {
            let msg = String::from(msgo).as_bytes_mut() as *mut [u8] as *mut u8;
            let p = msg.offset(msgo.len() as isize);
            let swp = *p;
            *p = 0;
            tty_text_print(tty, msg, u32::from(fgc), u32::from(bgc));
            *p = swp;
        }
    }
    loop {}
}
