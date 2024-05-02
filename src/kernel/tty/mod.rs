pub mod tty;

#[macro_export]
macro_rules! message_msg {
    () => {};
    ( $builder : expr, $e : expr) => {
        $builder.message_mut($e);
    };
}

#[macro_export]
macro_rules! message_fgc {
    () => {};
    ( $builder : expr, $e : expr) => {
        $builder.foreground_color_mut($e);
    };
}

#[macro_export]
macro_rules! message_bgc {
    () => {};
    ( $builder : expr, $e : expr ) => {
        $builder.background_color_mut($e);
    };
}

#[macro_export]
macro_rules! message_raw {
    ( $( $e : expr ),* ) => {{
        use crate::{
            kernel::tty::tty::{MessageBuilder, BuilderFunctions::*, Color},
            message_msg, message_fgc, message_bgc
        };
        let mut tmp_builder = MessageBuilder::new();
        $(
            if let Msg(e) = $e {
                message_msg!(tmp_builder, &e);
            } else if let FgColor(c) = $e {
                message_fgc!(tmp_builder, c);
            } else if let BgColor(c) = $e {
                message_bgc!(tmp_builder, c);
            }
        )*
        tmp_builder.build()
    }};
}

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
