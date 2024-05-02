use crate::{kernel::tty::tty::Tty, message};

extern "C" {
    pub fn interrupt_open();
    pub fn interrupt_close();
    // 以下两个函数签名的返回值用于使编译器保留rax寄存器
    pub fn interrupt_rust_enter() -> usize;
    pub fn interrupt_rust_leave() -> usize;
}

#[no_mangle]
unsafe extern "C" fn interrupt_req_UNSUPPORTED(rip: u64, rsp: u64, errcode: u64) -> ! {
    interrupt_rust_enter();
    let tty = Tty::from_id(0).unwrap();
    tty.enable();
    tty.print(message!(
        "{Panic}: Kernel hit an {Unsupported} interrupt on rip=0x{} and rsp=0x{}.\n",
        FmtMeta::Color(Color::RED),
        FmtMeta::Color(Color::YELLOW),
        FmtMeta::Pointer(rip as usize),
        FmtMeta::Pointer(rsp as usize)
    ));
    loop {}
}

#[no_mangle]
unsafe extern "C" fn interrupt_req_DE(rip: u64, rsp: u64, errcode: u64) {
    interrupt_rust_enter();
    let tty = Tty::from_id(0).unwrap();
    tty.enable();
    tty.print(message!(
        "{Warning}: Kernel hit {Divid Error} on rip=0x{} and rsp=0x{}.\n",
        FmtMeta::Color(Color::PURPLE),
        FmtMeta::Color(Color::YELLOW),
        FmtMeta::Pointer(rip as usize),
        FmtMeta::Pointer(rsp as usize)
    ));
    interrupt_rust_leave();
}

#[no_mangle]
unsafe extern "C" fn interrupt_req_NMI(rip: u64, rsp: u64, errcode: u64) {}

#[no_mangle]
unsafe extern "C" fn interrupt_req_BP(rip: u64, rsp: u64, errcode: u64) {}

#[no_mangle]
unsafe extern "C" fn interrupt_req_OF(rip: u64, rsp: u64, errcode: u64) {}
