use crate::message;

use super::{
    klog::{KernelLogger, LoggerLevel},
    tty::tty::{BuilderFunctions::*, Color, Tty},
};

#[no_mangle]
extern "C" fn kmain_rust() -> ! {
    let tty = Tty::from_id(0).unwrap();
    tty.enable();
    let mut logger = KernelLogger::new();
    logger.info(message!(
        Msg("Hello, "),
        Msg("Metaverse"),
        FgColor(Color(0xa, 0xee, 0xa)),
        Msg("!\n")
    ));
    for msg in logger.iter(LoggerLevel::Info) {
        tty.print(msg);
    }
    loop {}
}
