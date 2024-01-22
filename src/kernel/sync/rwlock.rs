use std::{
    ops::{Deref, DerefMut},
    ptr::null_mut,
};

/// ## RwLock<T>
/// 读写锁
/// 
/// * 对于写入操作：
/// 
/// 锁没有占用时，获取锁，并执行闭包。
/// 
/// 当锁已经被占用，将闭包悬挂。
/// 
/// 释放锁时，将所有悬挂的闭包都执行。
/// 
/// 正在释放锁时，`write`方法返回`Err(())`
///
/// ```
/// let num = 6;
/// let num = RwLock::new(num);
/// if let Err(()) = num.write(
///     |n| *n = 10
/// ) {}
/// let numstr = format!("{}", num.read());
/// ````
pub struct RwLock<'a, T> {
    obj: T,
    locked: bool,
    dealing_hanged: bool,
    ptr: *mut RwLockWriteGuard<'a, T>,
    hanging: Vec<&'a dyn Fn(&mut T)>,
}

unsafe impl<T> Send for RwLock<'_, T> {}
unsafe impl<T> Sync for RwLock<'_, T> {}

impl<'a, T> RwLock<'a, T> {
    pub fn new(obj: T) -> Self {
        Self {
            obj,
            locked: false,
            dealing_hanged: false,
            ptr: null_mut(),
            hanging: vec![],
        }
    }

    pub fn read(&self) -> RwLockReadGuard<T> {
        RwLockReadGuard { obj: &self.obj }
    }

    pub fn write(&'a mut self, f: &'a dyn Fn(&mut T)) -> Result<RwLockWriteGuard<T>, ()> {
        if self.dealing_hanged {
            Err(())
        } else if !self.locked {
            self.locked = true;
            f(&mut self.obj);
            let ptr = { self as *mut RwLock<'a, T> };
            let obj = &mut self.obj;
            let mut res = RwLockWriteGuard {
                obj,
                ptr,
                must_deal_hang: false,
            };
            self.ptr = &mut res as *mut RwLockWriteGuard<'_, T>;
            Ok(res)
        } else {
            self.hanging.push(f);
            let ptr = { self as *mut RwLock<'a, T> };
            let obj = &mut self.obj;
            Ok(RwLockWriteGuard {
                obj,
                ptr,
                must_deal_hang: false,
            })
        }
    }
}

pub struct RwLockReadGuard<'a, T> {
    obj: &'a T,
}

impl<'a, T> Deref for RwLockReadGuard<'a, T> {
    type Target = T;

    fn deref(&self) -> &Self::Target {
        self.obj
    }
}

impl<'a, T> Drop for RwLockReadGuard<'a, T> {
    fn drop(&mut self) {}
}

pub struct RwLockWriteGuard<'a, T> {
    obj: &'a mut T,
    ptr: *mut RwLock<'a, T>,
    must_deal_hang: bool,
}

impl<'a, T> Deref for RwLockWriteGuard<'a, T> {
    type Target = T;

    fn deref(&self) -> &Self::Target {
        self.obj
    }
}

impl<'a, T> DerefMut for RwLockWriteGuard<'a, T> {
    fn deref_mut(&mut self) -> &mut Self::Target {
        self.obj
    }
}

impl<'a, T> Drop for RwLockWriteGuard<'a, T> {
    fn drop(&mut self) {
        if self.must_deal_hang {
            let p = unsafe { &mut *self.ptr };
            p.dealing_hanged = true;
            for f in p.hanging.iter() {
                f(self.obj);
            }
            p.hanging.clear();
            p.dealing_hanged = false;
            p.locked = false;
        }
    }
}
