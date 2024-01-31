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
macro_rules! message {
    ( $( $e : expr ),* ) => {{
        use crate::{
            kernel::tty::tty::{MessageBuilder, BuilderFunctions},
            message_msg, message_fgc, message_bgc
        };
        let mut tmp_builder = MessageBuilder::new();
        $(
            if let BuilderFunctions::Msg(e) = $e {
                message_msg!(tmp_builder, e);
            } else if let BuilderFunctions::FgColor(c) = $e {
                message_fgc!(tmp_builder, c);
            } else if let BuilderFunctions::BgColor(c) = $e {
                message_bgc!(tmp_builder, c);
            }
        )*
        tmp_builder.build()
    }};
}
