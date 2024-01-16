extern crate core;

use core::alloc::{GlobalAlloc, Layout};

extern "C" {
    pub fn memm_allocate(size: usize, pid: usize) -> *mut u8;
    pub fn memm_free(mem: *mut u8);
}

pub struct KernelAllocator {}

unsafe impl GlobalAlloc for KernelAllocator {
    unsafe fn alloc(&self, layout: Layout) -> *mut u8 {
        memm_allocate(layout.size(), 0)
    }

    unsafe fn dealloc(&self, ptr: *mut u8, _layout: Layout) {
        memm_free(ptr);
    }
}

#[global_allocator]
static KERNEL_ALLOCATOR: KernelAllocator = KernelAllocator {};
