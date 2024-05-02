use core::mem::{size_of, transmute};

extern "C" {
    pub fn memset(des: *const u8, src: u8, len: usize);
    pub fn memcpy(des: *const u8, src: *const u8, len: usize);
    pub fn strlen(des: *const u8) -> usize;
}

pub trait PtrOptions<T> {
    unsafe fn kcopy_from(self, src: *const T, count: usize);
    unsafe fn kcopy_to(self, des: *const T, count: usize);
}

impl<T> PtrOptions<T> for *mut T {
    unsafe fn kcopy_from(self, src: *const T, count: usize) {
        memcpy(
            transmute(self.cast_const()),
            transmute(src),
            count * size_of::<T>(),
        );
    }

    unsafe fn kcopy_to(self, des: *const T, count: usize) {
        memcpy(
            transmute(des),
            transmute(self.cast_const()),
            count * size_of::<T>(),
        );
    }
}
