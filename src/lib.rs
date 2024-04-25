#![no_std]
#![feature(strict_provenance)]
#![feature(layout_for_ptr)]

extern crate alloc;

use core::panic::PanicInfo;

pub mod kernel;
pub mod libk;

#[panic_handler]
unsafe fn kernel_panic_handler(_info: &PanicInfo) -> ! {
    loop {}
}
