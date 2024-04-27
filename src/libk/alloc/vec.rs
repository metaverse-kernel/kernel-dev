use core::{
    alloc::Layout,
    ops::{Index, IndexMut, Range, RangeFull},
    ptr::addr_of_mut,
    slice,
};

use crate::libk::{
    alloc::alloc::{alloc, dealloc},
    core::ptr::PtrOptions,
};

pub struct Vec<T> {
    pointer: *mut T,
    length: usize,
    capacity: usize,
}

impl<T: Default> Vec<T> {
    pub fn new() -> Self {
        Self {
            pointer: unsafe { alloc(Layout::array::<T>(4).unwrap()).cast() },
            length: 0,
            capacity: 4,
        }
    }

    unsafe fn extend_capacity(&mut self) {
        let newp: *mut T = alloc(Layout::array::<T>(self.capacity * 2).unwrap()).cast();
        self.pointer.kcopy_to(newp, self.length);
        dealloc(
            self.pointer.cast(),
            Layout::array::<T>(self.capacity).unwrap(),
        );
        self.pointer = newp;
        self.capacity *= 2;
    }

    pub fn push(&mut self, item: T) {
        if self.capacity == self.length {
            unsafe { self.extend_capacity() }
        }
        unsafe { self.pointer.offset(self.length as isize).write(item) };
        self.length += 1;
    }

    pub fn insert(&mut self, index: usize, item: T) {
        if self.capacity == self.length {
            unsafe { self.extend_capacity() }
        }
        let rearlen = self.length - index;
        unsafe {
            if rearlen != 0 {
                let tmp = alloc(Layout::array::<T>(rearlen).unwrap()).cast();
                self.pointer.offset(index as isize).kcopy_to(tmp, rearlen);
                self.pointer.offset(index as isize).write(item);
                self.pointer
                    .offset(index as isize + 1)
                    .kcopy_from(tmp, rearlen);
            } else {
                self.pointer.offset(self.length as isize).write(item);
            }
        }
        self.length += 1;
    }

    pub fn append(&mut self, v: &mut Self) {
        while self.capacity < self.length + v.length {
            unsafe { self.extend_capacity() }
        }
        unsafe {
            self.pointer
                .offset(self.length as isize)
                .kcopy_from(v.pointer.cast_const(), v.length)
        };
        self.length += v.length;
    }

    pub fn split_at(&self, index: usize) -> (&[T], &[T]) {
        if index >= self.length {
            panic!("Index out of bound.");
        }
        (&self[0..index], &self[index..self.length])
    }

    pub fn len(&self) -> usize {
        self.length
    }

    pub fn is_empty(&self) -> bool {
        self.length == 0
    }

    pub fn last_mut(&mut self) -> Option<&mut T> {
        unsafe { self.pointer.offset(self.length as isize - 1).as_mut() }
    }

    pub fn last(&self) -> Option<&T> {
        unsafe { self.pointer.offset(self.length as isize - 1).as_ref() }
    }

    pub fn remove(&mut self, index: usize) -> T {
        if index >= self.length {
            panic!("Index out of bound.");
        }
        let mut t: T = T::default();
        let pt = addr_of_mut!(t);
        unsafe {
            pt.kcopy_from(self.pointer.offset(index as isize).cast_const(), 1);
            self.pointer.offset(index as isize).kcopy_from(
                self.pointer.offset(index as isize + 1),
                self.length - index - 1,
            );
        }
        t
    }

    pub fn iter(&self) -> VecIterator<T> {
        VecIterator {
            pointer: self.pointer,
            index: 0,
            length: self.length,
            _phantom: &(),
        }
    }
}

impl<T: Default> Default for Vec<T> {
    fn default() -> Self {
        Self::new()
    }
}

impl<T: Default> Index<usize> for Vec<T> {
    type Output = T;

    fn index(&self, index: usize) -> &Self::Output {
        unsafe { self.pointer.offset(index as isize).as_ref().unwrap() }
    }
}

impl<T: Default> Index<Range<usize>> for Vec<T> {
    type Output = [T];

    fn index(&self, index: Range<usize>) -> &Self::Output {
        unsafe { &slice::from_raw_parts_mut(self.pointer, self.length)[index] }
    }
}

impl<T: Default> Index<RangeFull> for Vec<T> {
    type Output = [T];

    fn index(&self, _: RangeFull) -> &Self::Output {
        unsafe { slice::from_raw_parts(self.pointer.cast(), self.length) }
    }
}

impl<T: Default> IndexMut<RangeFull> for Vec<T> {
    fn index_mut(&mut self, _: RangeFull) -> &mut Self::Output {
        unsafe { slice::from_raw_parts_mut(self.pointer.cast(), self.length) }
    }
}

impl<T: Default> Clone for Vec<T> {
    fn clone(&self) -> Self {
        let res = Self {
            pointer: unsafe { alloc(Layout::array::<T>(self.capacity).unwrap()).cast() },
            length: self.length,
            capacity: self.capacity,
        };
        unsafe {
            res.pointer
                .kcopy_from(self.pointer.cast_const(), self.length)
        };
        res
    }
}

impl<T> Drop for Vec<T> {
    fn drop(&mut self) {
        unsafe {
            dealloc(
                self.pointer.cast(),
                Layout::array::<T>(self.capacity).unwrap(),
            )
        };
    }
}

impl<T: Default> FromIterator<T> for Vec<T> {
    fn from_iter<U: IntoIterator<Item = T>>(iter: U) -> Self {
        let mut res = Vec::new();
        for i in iter {
            res.push(i);
        }
        res
    }
}

impl<T: Default> IntoIterator for Vec<T> {
    type Item = T;

    type IntoIter = VecIter<T>;

    fn into_iter(self) -> Self::IntoIter {
        VecIter {
            pointer: self.pointer,
            index: 0,
            length: self.length,
            capacity: self.capacity,
        }
    }
}

pub struct VecIterator<'a, T> {
    pointer: *mut T,
    index: usize,
    length: usize,
    _phantom: &'a (),
}

impl<'a, T: 'a> Iterator for VecIterator<'a, T> {
    type Item = &'a T;

    fn next(&mut self) -> Option<Self::Item> {
        if self.index == self.length {
            None
        } else {
            let res = unsafe { Some(self.pointer.offset(self.index as isize).as_ref().unwrap()) };
            self.index += 1;
            res
        }
    }
}

impl<'a, T: 'a> DoubleEndedIterator for VecIterator<'a, T> {
    fn next_back(&mut self) -> Option<Self::Item> {
        if self.index == 0 {
            None
        } else {
            self.index -= 1;
            unsafe { Some(self.pointer.offset(self.index as isize).as_ref().unwrap()) }
        }
    }
}

pub struct VecIter<T> {
    pointer: *mut T,
    index: usize,
    length: usize,
    capacity: usize,
}

impl<T: Default> Iterator for VecIter<T> {
    type Item = T;

    fn next(&mut self) -> Option<Self::Item> {
        if self.index == self.length {
            None
        } else {
            let mut t = T::default();
            let res = unsafe {
                self.pointer
                    .offset(self.index as isize)
                    .kcopy_to(addr_of_mut!(t), 1);
                Some(t)
            };
            self.index += 1;
            res
        }
    }
}

impl<T> Drop for VecIter<T> {
    fn drop(&mut self) {
        unsafe {
            dealloc(
                self.pointer.cast(),
                Layout::array::<T>(self.capacity).unwrap(),
            )
        };
    }
}
