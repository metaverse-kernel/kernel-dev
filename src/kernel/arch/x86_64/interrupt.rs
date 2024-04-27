use crate::{kernel::tty::tty::Tty, libk::alloc::string::ToString, message, message_raw};

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
    let msg = message_raw!(
        Msg("Panic:"),
        FgColor::<u8>(Color::RED),
        Msg(" Kernel hit an unsupported interrupt\n\twith %rip=0x"),
        Msg(rip.to_hex_string()),
        Msg(" and %rsp=0x"),
        Msg(rsp.to_hex_string())
    );
    tty.print(msg);
    loop {}
}

#[no_mangle]
unsafe extern "C" fn interrupt_req_DE(rip: u64, rsp: u64, errcode: u64) {
    interrupt_rust_enter();
    let tty = Tty::from_id(0).unwrap();
    tty.enable();
    tty.print(message!(
        "{Panic}: divided by zero. rip=0x{}.\n",
        FmtMeta::Color(Color::RED),
        FmtMeta::Pointer(rip as usize)
    ));
    loop {}
    interrupt_rust_leave();
}

#[no_mangle]
unsafe extern "C" fn interrupt_req_NMI(rip: u64, rsp: u64, errcode: u64) {}

#[no_mangle]
unsafe extern "C" fn interrupt_req_BP(rip: u64, rsp: u64, errcode: u64) {}

#[no_mangle]
unsafe extern "C" fn interrupt_req_OF(rip: u64, rsp: u64, errcode: u64) {}
