use alloc::string::{String, ToString};

#[macro_export]
macro_rules! format {
    ( $s : expr, $( $e : expr ),* ) => {{
        use crate::libk::string::format::Format;
        let mut res = $s.to_string();
        $(
            res.format($e);
        )*
        res
    }};
    () => {};
}

pub trait Format<T: ToString> {
    fn format(&mut self, f: T);
}

impl<T: ToString> Format<T> for String {
    fn format(&mut self, f: T) {
        let mut res = String::new();
        let mut formatting = false;
        for c in self.chars().into_iter() {
            if c == '{' {
                formatting = true;
                res += &f.to_string();
            } else if c == '}' {
                formatting = false;
            }
            if !formatting {
                res.push(c);
            }
        }
        self.clear();
        self.push_str(&res);
    }
}
