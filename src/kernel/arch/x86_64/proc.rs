#[repr(C)]
#[derive(Debug)]
pub struct RegisterTexture {
    es: u16,
    ds: u16,
    reserved: u32,
    r15: u64,
    r14: u64,
    r13: u64,
    r12: u64,
    r11: u64,
    r10: u64,
    r9: u64,
    r8: u64,
    rdi: u64,
    rsi: u64,
    rdx: u64,
    rbx: u64,
    rcx: u64,
    rax: u64,
    rbp: u64,
    rip: u64,
    cs: u64,
    rflags: u64,
    rsp: u64,
    ss: u64,
}

#[derive(Debug)]
pub struct ProcessTexture {
    register: RegisterTexture,
}
