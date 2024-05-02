pub mod tty;

#[macro_export]
macro_rules! message {
    ( $fmtter : expr ) => {{
        use crate::kernel::tty::tty::MessageBuilder;
        MessageBuilder::new()
            .message($fmtter)
            .build()
    }};
    ( $fmtter : expr, $( $e : expr ),* ) => {{
        use crate::{
            kernel::tty::tty::{
                MessageBuilder,
                FmtMeta,
                Color,
                format_message
            },
            libk::alloc::vec::Vec,
        };
        let mut formatter = $fmtter.chars().collect::<Vec<char>>();
        let builder = MessageBuilder::new();
        $(
            let builder = builder.append(format_message(&mut formatter, $e));
        )*
        builder.build()
    }};
}
