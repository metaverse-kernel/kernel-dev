use alloc::string::ToString;
use alloc::vec;
use alloc::vec::Vec;

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

    pub fn iter(&self, level: LoggerLevel) -> LogIterator {
        let mut logs = vec![];
        match level {
            LoggerLevel::Fatal => {
                logs.push(&self.fatal_queue);
            }
            LoggerLevel::Error => {
                logs.push(&self.fatal_queue);
                logs.push(&self.error_queue);
            }
            LoggerLevel::Warning => {
                logs.push(&self.fatal_queue);
                logs.push(&self.error_queue);
                logs.push(&self.warning_queue);
            }
            LoggerLevel::Info => {
                logs.push(&self.fatal_queue);
                logs.push(&self.error_queue);
                logs.push(&self.warning_queue);
                logs.push(&self.info_queue);
            }
            LoggerLevel::Debug => {
                logs.push(&self.fatal_queue);
                logs.push(&self.error_queue);
                logs.push(&self.warning_queue);
                logs.push(&self.info_queue);
                logs.push(&self.debug_queue);
            }
            LoggerLevel::Trace => {
                logs.push(&self.fatal_queue);
                logs.push(&self.error_queue);
                logs.push(&self.warning_queue);
                logs.push(&self.info_queue);
                logs.push(&self.debug_queue);
                logs.push(&self.trace_queue);
            }
        }
        let mut res = vec![];
        let mut indeces = Vec::new();
        for _ in 0..logs.len() {
            indeces.push(0usize);
        }
        let all_end = |indeces: &Vec<usize>, logs: &Vec<&Vec<(SystemTime, Message)>>| {
            for i in 0..indeces.len() {
                if indeces[i] < logs.len() {
                    return false;
                }
            }
            return true;
        };
        while !all_end(&indeces, &logs) {
            let mut min_ind = None;
            let mut min = None;
            for i in 0..indeces.len() {
                if indeces[i] >= logs[i].len() {
                    continue;
                }
                if let Some(minx) = min.as_mut() {
                    if logs[i][indeces[i]].0 < *minx {
                        *minx = logs[i][indeces[i]].0;
                        min_ind = Some(i);
                    }
                } else {
                    min = Some(logs[i][indeces[i]].0);
                    min_ind = Some(i);
                }
            }
            if let Some(mini) = min_ind {
                res.push(&logs[mini][indeces[mini]]);
                indeces[mini] += 1;
            } else {
                break;
            }
        }
        LogIterator { logs: res }
    }
}

pub struct LogIterator<'a> {
    logs: Vec<&'a (SystemTime, Message)>,
}

impl<'a> Iterator for LogIterator<'a> {
    type Item = Message;

    fn next(&mut self) -> Option<Self::Item> {
        let res = if let Some((time, msg)) = self.logs.first() {
            Some(
                MessageBuilder::new()
                    .message(&time.to_string())
                    .append(MessageBuilder::from_message(msg.clone()))
                    .build(),
            )
        } else {
            None
        };
        if let Some(_) = res {
            self.logs.remove(0);
        }
        res
    }
}
