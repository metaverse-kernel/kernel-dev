use core::ops::{Add, AddAssign};

use super::vec::{Vec, VecIterator};

#[derive(Default)]
pub struct String {
    data: Vec<char>,
    u8data: Vec<u8>,
}

impl String {
    pub fn new() -> Self {
        Self {
            data: Vec::new(),
            u8data: Vec::new(),
        }
    }

    pub fn len(&self) -> usize {
        self.data.len()
    }

    pub fn chars(&self) -> VecIterator<char> {
        self.data.iter()
    }

    pub fn as_bytes(&self) -> &[u8] {
        &self.u8data[..]
    }

    pub unsafe fn as_bytes_mut(&mut self) -> &mut [u8] {
        &mut self.u8data[..]
    }

    pub fn insert(&mut self, index: usize, item: char) {
        self.data.insert(index, item);
        self.u8data.insert(index, item as u8);
    }
}

impl FromIterator<char> for String {
    fn from_iter<T: IntoIterator<Item = char>>(iter: T) -> Self {
        let data = Vec::from_iter(iter);
        let u8data = Vec::from_iter(data.iter().map(|c| *c as u8));
        Self { data, u8data }
    }
}

impl<'a> FromIterator<&'a char> for String {
    fn from_iter<T: IntoIterator<Item = &'a char>>(iter: T) -> Self {
        let data = Vec::from_iter(iter.into_iter().map(|c| *c));
        let u8data = Vec::from_iter(data.iter().map(|c| *c as u8));
        Self { data, u8data }
    }
}

impl ToString for String {
    fn to_string(&self) -> String {
        self.clone()
    }
}

impl Add for String {
    type Output = Self;

    fn add(mut self, rhs: Self) -> Self::Output {
        self.data.append(&mut rhs.chars().map(|c| *c).collect());
        self
    }
}

impl AddAssign for String {
    fn add_assign(&mut self, rhs: String) {
        *self = self.clone() + rhs;
    }
}

impl Clone for String {
    fn clone(&self) -> Self {
        Self {
            data: self.data.clone(),
            u8data: self.u8data.clone(),
        }
    }
}

pub trait ToString {
    fn to_string(&self) -> String;
    fn to_octal_string(&self) -> String {
        String::new()
    }
    fn to_hex_string(&self) -> String {
        String::new()
    }
}

impl ToString for str {
    fn to_string(&self) -> String {
        String::from_iter(self.chars())
    }
}

impl ToString for &'static str {
    fn to_string(&self) -> String {
        (*self).to_string()
    }
}

impl ToString for u8 {
    fn to_string(&self) -> String {
        let mut num = *self;
        let mut res = String::new();
        while num != 0 {
            res.insert(0, ((num % 10) as u8 + b'0') as char);
            num /= 10;
        }
        res
    }

    fn to_octal_string(&self) -> String {
        let mut num = *self;
        let mut res = String::new();
        while num != 0 {
            res.insert(0, ((num % 8) as u8 + b'0') as char);
            num /= 8;
        }
        res
    }

    fn to_hex_string(&self) -> String {
        let tohex = |x| {
            if x < 10 {
                x as u8 + b'0'
            } else {
                x as u8 - 10 + b'a'
            }
        };
        let mut num = *self;
        let mut res = String::new();
        while num != 0 {
            res.insert(0, tohex(num % 16) as char);
            num /= 16;
        }
        res
    }
}

impl ToString for u16 {
    fn to_string(&self) -> String {
        let mut num = *self;
        let mut res = String::new();
        while num != 0 {
            res.insert(0, ((num % 10) as u8 + b'0') as char);
            num /= 10;
        }
        if res.len() == 0 {
            res.insert(0, '0');
        }
        res
    }

    fn to_octal_string(&self) -> String {
        let mut num = *self;
        let mut res = String::new();
        while num != 0 {
            res.insert(0, ((num % 8) as u8 + b'0') as char);
            num /= 8;
        }
        if res.len() == 0 {
            res.insert(0, '0');
        }
        res
    }

    fn to_hex_string(&self) -> String {
        let tohex = |x| {
            if x < 10 {
                x as u8 + b'0'
            } else {
                x as u8 - 10 + b'a'
            }
        };
        let mut num = *self;
        let mut res = String::new();
        while num != 0 {
            res.insert(0, tohex(num % 16) as char);
            num /= 16;
        }
        if res.len() == 0 {
            res.insert(0, '0');
        }
        res
    }
}

impl ToString for u32 {
    fn to_string(&self) -> String {
        let mut num = *self;
        let mut res = String::new();
        while num != 0 {
            res.insert(0, ((num % 10) as u8 + b'0') as char);
            num /= 10;
        }
        if res.len() == 0 {
            res.insert(0, '0');
        }
        res
    }

    fn to_octal_string(&self) -> String {
        let mut num = *self;
        let mut res = String::new();
        while num != 0 {
            res.insert(0, ((num % 8) as u8 + b'0') as char);
            num /= 8;
        }
        if res.len() == 0 {
            res.insert(0, '0');
        }
        res
    }

    fn to_hex_string(&self) -> String {
        let tohex = |x| {
            if x < 10 {
                x as u8 + b'0'
            } else {
                x as u8 - 10 + b'a'
            }
        };
        let mut num = *self;
        let mut res = String::new();
        while num != 0 {
            res.insert(0, tohex(num % 16) as char);
            num /= 16;
        }
        if res.len() == 0 {
            res.insert(0, '0');
        }
        res
    }
}

impl ToString for u64 {
    fn to_string(&self) -> String {
        let mut num = *self;
        let mut res = String::new();
        while num != 0 {
            res.insert(0, ((num % 10) as u8 + b'0') as char);
            num /= 10;
        }
        if res.len() == 0 {
            res.insert(0, '0');
        }
        res
    }

    fn to_octal_string(&self) -> String {
        let mut num = *self;
        let mut res = String::new();
        while num != 0 {
            res.insert(0, ((num % 8) as u8 + b'0') as char);
            num /= 8;
        }
        if res.len() == 0 {
            res.insert(0, '0');
        }
        res
    }

    fn to_hex_string(&self) -> String {
        let tohex = |x| {
            if x < 10 {
                x as u8 + b'0'
            } else {
                x as u8 - 10 + b'a'
            }
        };
        let mut num = *self;
        let mut res = String::new();
        while num != 0 {
            res.insert(0, tohex(num % 16) as char);
            num /= 16;
        }
        if res.len() == 0 {
            res.insert(0, '0');
        }
        res
    }
}

impl ToString for u128 {
    fn to_string(&self) -> String {
        let mut num = *self;
        let mut res = String::new();
        while num != 0 {
            res.insert(0, ((num % 10) as u8 + b'0') as char);
            num /= 10;
        }
        if res.len() == 0 {
            res.insert(0, '0');
        }
        res
    }

    fn to_octal_string(&self) -> String {
        let mut num = *self;
        let mut res = String::new();
        while num != 0 {
            res.insert(0, ((num % 8) as u8 + b'0') as char);
            num /= 8;
        }
        if res.len() == 0 {
            res.insert(0, '0');
        }
        res
    }

    fn to_hex_string(&self) -> String {
        let tohex = |x| {
            if x < 10 {
                x as u8 + b'0'
            } else {
                x as u8 - 10 + b'a'
            }
        };
        let mut num = *self;
        let mut res = String::new();
        while num != 0 {
            res.insert(0, tohex(num % 16) as char);
            num /= 16;
        }
        if res.len() == 0 {
            res.insert(0, '0');
        }
        res
    }
}

impl ToString for usize {
    fn to_string(&self) -> String {
        let mut num = *self;
        let mut res = String::new();
        while num != 0 {
            res.insert(0, ((num % 10) as u8 + b'0') as char);
            num /= 10;
        }
        if res.len() == 0 {
            res.insert(0, '0');
        }
        res
    }

    fn to_octal_string(&self) -> String {
        let mut num = *self;
        let mut res = String::new();
        while num != 0 {
            res.insert(0, ((num % 8) as u8 + b'0') as char);
            num /= 8;
        }
        if res.len() == 0 {
            res.insert(0, '0');
        }
        res
    }

    fn to_hex_string(&self) -> String {
        let tohex = |x| {
            if x < 10 {
                x as u8 + b'0'
            } else {
                x as u8 - 10 + b'a'
            }
        };
        let mut num = *self;
        let mut res = String::new();
        while num != 0 {
            res.insert(0, tohex(num % 16) as char);
            num /= 16;
        }
        if res.len() == 0 {
            res.insert(0, '0');
        }
        res
    }
}
