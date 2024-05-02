use core::alloc::{GlobalAlloc, Layout};

use crate::kernel::memm::memm::KERNEL_ALLOCATOR;

pub unsafe fn alloc(layout: Layout) -> *mut u8 {
    KERNEL_ALLOCATOR.alloc(layout)
}

pub unsafe fn dealloc(ptr: *mut u8, layout: Layout) {
    KERNEL_ALLOCATOR.dealloc(ptr, layout);
}
