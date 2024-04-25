use crate::{
    kernel::{
        klog::{KernelLogger, LoggerLevel},
        tty::tty::Tty,
    },
    message_raw,
};

#[no_mangle]
extern "C" fn kmain_rust() -> ! {
    let tty = Tty::from_id(0).unwrap();
    loop {}
}
