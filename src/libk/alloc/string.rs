use core::ops::{Add, AddAssign};

use super::vec::{Vec, VecIterator};

#[derive(Clone, Default)]
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

pub trait ToString {
    fn to_string(&self) -> String;
}

impl ToString for str {
    fn to_string(&self) -> String {
        String::from_iter(self.chars())
    }
}
