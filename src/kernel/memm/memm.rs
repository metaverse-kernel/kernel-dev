extern crate core;

use core::{
    alloc::{GlobalAlloc, Layout},
    ptr::null_mut,
};

extern "C" {
    fn memm_kernel_allocate(size: usize) -> *mut u8;
    // pub fn memm_user_allocate(size: usize, pid: usize);
    fn memm_free(mem: *mut u8);
}

pub struct KernelAllocator {}

unsafe impl GlobalAlloc for KernelAllocator {
    unsafe fn alloc(&self, layout: Layout) -> *mut u8 {
        let res = memm_kernel_allocate(layout.size());
        if res == null_mut() {
            panic!(
                "Kernel allocator failed to allocate {} byte(s) memory.",
                layout.size()
            );
        }
        res
    }

    unsafe fn dealloc(&self, ptr: *mut u8, _layout: Layout) {
        memm_free(ptr);
    }
}

#[global_allocator]
pub static KERNEL_ALLOCATOR: KernelAllocator = KernelAllocator {};
