use alloc::string::ToString;
use alloc::{format, vec::Vec};
use alloc::vec;

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
            fatal_queue: vec![],
            error_queue: vec![],
            warning_queue: vec![],
            info_queue: vec![],
            debug_queue: vec![],
            trace_queue: vec![],
        }
    }

    pub fn fatal(&mut self, msg: Message) {
        let msg = MessageBuilder::new()
            .message("Fatal: ".to_string())
            .foreground_color(Color(0xee, 0xa, 0xa))
            .append(MessageBuilder::from_message(&msg))
            .build();
        self.fatal_queue.push((SystemTime::now(), msg));
    }

    pub fn error(&mut self, msg: Message) {
        let msg = MessageBuilder::new()
            .message("Error: ".to_string())
            .foreground_color(Color(0xaa, 0x22, 0x22))
            .append(MessageBuilder::from_message(&msg))
            .build();
        self.error_queue.push((SystemTime::now(), msg));
    }

    pub fn warning(&mut self, msg: Message) {
        let msg = MessageBuilder::new()
            .message("Warning: ".to_string())
            .foreground_color(Color(0xaa, 0xa, 0xaa))
            .append(MessageBuilder::from_message(&msg))
            .build();
        self.warning_queue.push((SystemTime::now(), msg));
    }

    pub fn info(&mut self, msg: Message) {
        let msg = MessageBuilder::new()
            .message("Info: ".to_string())
            .foreground_color(Color(0xa, 0xee, 0xa))
            .append(MessageBuilder::from_message(&msg))
            .build();
        self.info_queue.push((SystemTime::now(), msg));
    }

    pub fn debug(&mut self, msg: Message) {
        let msg = MessageBuilder::new()
            .message("Debug: ".to_string())
            .foreground_color(Color(0xee, 0xee, 0xee))
            .append(MessageBuilder::from_message(&msg))
            .build();
        self.debug_queue.push((SystemTime::now(), msg));
    }

    pub fn trace(&mut self, msg: Message) {
        let msg = MessageBuilder::new()
            .message("Trace: ".to_string())
            .foreground_color(Color(0xee, 0xee, 0xee))
            .append(MessageBuilder::from_message(&msg))
            .build();
        self.trace_queue.push((SystemTime::now(), msg));
    }

    pub fn iter(&self, level: LoggerLevel) -> LogIterator {
        let mut logs = vec![];
        if level == LoggerLevel::Fatal {
            logs.push(&self.fatal_queue);
        }
        if level == LoggerLevel::Fatal || level == LoggerLevel::Error {
            logs.push(&self.error_queue);
        }
        if level == LoggerLevel::Fatal
            || level == LoggerLevel::Error
            || level == LoggerLevel::Warning
        {
            logs.push(&self.warning_queue);
        }
        if level == LoggerLevel::Fatal
            || level == LoggerLevel::Error
            || level == LoggerLevel::Warning
            || level == LoggerLevel::Info
        {
            logs.push(&self.info_queue);
        }
        if level == LoggerLevel::Fatal
            || level == LoggerLevel::Error
            || level == LoggerLevel::Warning
            || level == LoggerLevel::Info
            || level == LoggerLevel::Debug
        {
            logs.push(&self.debug_queue);
        }
        if level == LoggerLevel::Fatal
            || level == LoggerLevel::Error
            || level == LoggerLevel::Warning
            || level == LoggerLevel::Info
            || level == LoggerLevel::Debug
            || level == LoggerLevel::Trace
        {
            logs.push(&self.trace_queue);
        }
        let mut res = vec![];
        while let Some(msg) = {
            logs.iter_mut()
                .filter_map(|&mut l| l.first().cloned())
                .min_by_key(|&(syst, _)| syst)
        } {
            res.push(msg);
        }
        LogIterator { logs: res }
    }
}

pub struct LogIterator {
    logs: Vec<(SystemTime, Message)>,
}

impl Iterator for LogIterator {
    type Item = Message;

    fn next(&mut self) -> Option<Self::Item> {
        if let Some((time, msg)) = self.logs.first() {
            Some(
                MessageBuilder::new()
                    .message(format!("{}", time.to_string()))
                    .append(MessageBuilder::from_message(msg))
                    .build(),
            )
        } else {
            None
        }
    }
}
