use core::{
    alloc::Layout,
    mem::transmute,
    ops::{Deref, DerefMut},
    ptr::addr_of,
};

use crate::libk::alloc::alloc::{alloc, dealloc};

pub struct Box<T: ?Sized> {
    inner: *mut T,
}

impl<T> Box<T> {
    pub fn new(t: T) -> Self {
        unsafe {
            let inner = alloc(Layout::for_value(&t));
            inner.copy_from(addr_of!(t).cast(), 1);
            Self {
                inner: transmute(inner),
            }
        }
    }
}

impl<T: ?Sized> Deref for Box<T> {
    type Target = T;

    fn deref(&self) -> &Self::Target {
        unsafe { self.inner.as_ref().unwrap() }
    }
}

impl<T: ?Sized> DerefMut for Box<T> {
    fn deref_mut(&mut self) -> &mut Self::Target {
        unsafe { self.inner.as_mut().unwrap() }
    }
}

impl<T: ?Sized> Drop for Box<T> {
    fn drop(&mut self) {
        unsafe {
            dealloc(
                self.inner.cast(),
                Layout::for_value_raw(self.inner.cast_const()),
            )
        }
    }
}
