use std::panic;

use crate::kernel::tty::tty::{Color, MessageBuilder, Tty};

#[no_mangle]
extern "C" fn kmain_rust() {
    let tty = Tty::from_id(0).unwrap();
    // let ttyptr = unsafe { tty.get_c_tty_t() };

    let hello = MessageBuilder::new()
        .message("Hello, ".to_string())
        .message("Metaverse".to_string())
        .foreground_color(Color(0xa, 0xee, 0xa))
        .message("!\n".to_string())
        .build();
    tty.print(hello);
    loop {}
}
