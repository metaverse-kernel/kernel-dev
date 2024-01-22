use super::{
    klog::{KernelLogger, LoggerLevel},
    tty::tty::{Color, ColorPair, Message, Tty},
};

#[no_mangle]
extern "C" fn kmain_rust() {
    let mut logger = KernelLogger::new();
    logger.info(Message::from(format!(
        "Hello, {:?}Metaverse{:?}!",
        ColorPair::color(Color(0xa, 0xee, 0xa), Color(0, 0, 0)),
        ColorPair::Reset
    )));
    let tty = Tty::from_id(0).unwrap();
    for msg in logger.iter(LoggerLevel::Info) {
        tty.print(msg);
    }
    loop {}
}
