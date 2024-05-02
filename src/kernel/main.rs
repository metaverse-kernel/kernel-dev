use crate::kernel::tty::tty::Tty;

#[no_mangle]
extern "C" fn kmain_rust() -> ! {
    let tty = Tty::from_id(0).unwrap();
    loop {}
}
