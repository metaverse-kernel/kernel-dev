use crate::libk::alloc::vec::Vec;

use super::{
    clock::time::SystemTime,
    tty::tty::{Color, Message, MessageBuilder},
};

#[derive(PartialEq)]
pub enum LoggerLevel {
    Fatal,
    Error,
    Warning,
    Info,
    Debug,
    Trace,
}

pub struct KernelLogger {
    fatal_queue: Vec<(SystemTime, Message)>,
    error_queue: Vec<(SystemTime, Message)>,
    warning_queue: Vec<(SystemTime, Message)>,
    info_queue: Vec<(SystemTime, Message)>,
    debug_queue: Vec<(SystemTime, Message)>,
    trace_queue: Vec<(SystemTime, Message)>,
}

impl KernelLogger {
    pub fn new() -> Self {
        Self {
            fatal_queue: Vec::new(),
            error_queue: Vec::new(),
            warning_queue: Vec::new(),
            info_queue: Vec::new(),
            debug_queue: Vec::new(),
            trace_queue: Vec::new(),
        }
    }

    pub fn fatal(&mut self, msg: Message) {
        let msg = MessageBuilder::new()
            .message("Fatal: ")
            .foreground_color(Color::RED)
            .append(MessageBuilder::from_message(msg))
            .build();
        self.fatal_queue.push((SystemTime::now(), msg));
    }

    pub fn error(&mut self, msg: Message) {
        let msg = MessageBuilder::new()
            .message("Error: ")
            .foreground_color(Color::ORANGE)
            .append(MessageBuilder::from_message(msg))
            .build();
        self.error_queue.push((SystemTime::now(), msg));
    }

    pub fn warning(&mut self, msg: Message) {
        let msg = MessageBuilder::new()
            .message("Warning: ")
            .foreground_color(Color::PURPLE)
            .append(MessageBuilder::from_message(msg))
            .build();
        self.warning_queue.push((SystemTime::now(), msg));
    }

    pub fn info(&mut self, msg: Message) {
        let msg = MessageBuilder::new()
            .message("Info: ")
            .foreground_color(Color::GREEN)
            .append(MessageBuilder::from_message(msg))
            .build();
        self.info_queue.push((SystemTime::now(), msg));
    }

    pub fn debug(&mut self, msg: Message) {
        let msg = MessageBuilder::new()
            .message("Debug: ")
            .foreground_color(Color::WHITE)
            .append(MessageBuilder::from_message(msg))
            .build();
        self.debug_queue.push((SystemTime::now(), msg));
    }

    pub fn trace(&mut self, msg: Message) {
        let msg = MessageBuilder::new()
            .message("Trace: ")
            .foreground_color(Color::WHITE)
            .append(MessageBuilder::from_message(msg))
            .build();
        self.trace_queue.push((SystemTime::now(), msg));
    }

    pub fn iter(&self, _level: LoggerLevel) -> LogIterator {
        todo!()
    }
}

pub struct LogIterator {
}

impl Iterator for LogIterator {
    type Item = Message;

    fn next(&mut self) -> Option<Self::Item> {
        todo!()
    }
}
