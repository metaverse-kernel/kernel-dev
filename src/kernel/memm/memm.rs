extern crate core;

use core::alloc::{GlobalAlloc, Layout};

extern "C" {
    pub fn memm_kernel_allocate(size: usize) -> *mut u8;
    // pub fn memm_user_allocate(size: usize, pid: usize);
    pub fn memm_free(mem: *mut u8);
}

pub struct KernelAllocator {}

unsafe impl GlobalAlloc for KernelAllocator {
    unsafe fn alloc(&self, layout: Layout) -> *mut u8 {
        memm_kernel_allocate(layout.size())
    }

    unsafe fn dealloc(&self, ptr: *mut u8, _layout: Layout) {
        memm_free(ptr);
    }
}

#[global_allocator]
static KERNEL_ALLOCATOR: KernelAllocator = KernelAllocator {};
