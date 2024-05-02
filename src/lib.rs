#![no_std]
#![feature(strict_provenance)]
#![feature(layout_for_ptr)]

extern crate alloc;

use core::panic::PanicInfo;

use kernel::tty::tty::Tty;

pub mod kernel;
pub mod libk;

#[panic_handler]
unsafe fn kernel_panic_handler(_info: &PanicInfo) -> ! {
    let tty = Tty::from_id(0).unwrap();
    tty.enable();
    tty.print(message!(
        "Hit a {Rust} panic.",
        FmtMeta::Color(Color::ORANGE)
    ));
    loop {}
}
