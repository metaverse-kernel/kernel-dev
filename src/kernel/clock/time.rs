use core::{cmp::Ordering, ops::Sub, time::Duration};

use alloc::string::ToString;

use crate::format;

extern "C" {
    fn system_time_get() -> usize;
    fn system_time_ns_get() -> usize;
}

#[derive(Debug)]
pub struct SystemTimeError(Duration);

#[derive(Clone, Copy, Hash)]
pub struct SystemTime {
    unix_time: usize,
    ns_time: usize,
}

impl ToString for SystemTime {
    fn to_string(&self) -> alloc::string::String {
        let second_dur = 1000usize;
        let minute_dur = second_dur * 60;
        let hour_dur = minute_dur * 60;
        let day_dur = hour_dur * 24;
        let year_dur = day_dur * 365;
        let four_year_dur = day_dur * (365 * 4 + 1);

        let year = 1970
            + self.unix_time / four_year_dur * 4
            + if self.unix_time % four_year_dur < day_dur * 59 {
                0
            } else {
                (self.unix_time % four_year_dur - day_dur) / year_dur
            };
        let rest = self.unix_time % four_year_dur;
        let mut leap = false;
        let rest = if rest < day_dur * 59 {
            rest
        } else {
            if rest < 60 {
                leap = true;
            }
            rest - day_dur
        };
        let month = rest % year_dur;
        let mut day = 0;
        let month = if month < 31 * day_dur {
            day = month;
            1
        } else if month < (31 + 28) * day_dur {
            day = month - 31 * day_dur;
            2
        } else if month < (31 + 28 + 31) * day_dur {
            day = month - (31 + 28) * day_dur;
            3
        } else if month < (31 + 28 + 31 + 30) * day_dur {
            day = month - (31 + 28 + 31) * day_dur;
            4
        } else if month < (31 + 28 + 31 + 30 + 31) * day_dur {
            day = month - (31 + 28 + 31 + 30) * day_dur;
            5
        } else if month < (31 + 28 + 31 + 30 + 31 + 30) * day_dur {
            day = month - (31 + 28 + 31 + 30 + 31) * day_dur;
            6
        } else if month < (31 + 28 + 31 + 30 + 31 + 30 + 31) * day_dur {
            day = month - (31 + 28 + 31 + 30 + 31 + 30) * day_dur;
            7
        } else if month < (31 + 28 + 31 + 30 + 31 + 30 + 31 + 31) * day_dur {
            day = month - (31 + 28 + 31 + 30 + 31 + 30 + 31) * day_dur;
            8
        } else if month < (31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30) * day_dur {
            day = month - (31 + 28 + 31 + 30 + 31 + 30 + 31 + 31) * day_dur;
            9
        } else if month < (31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31) * day_dur {
            day = month - (31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30) * day_dur;
            10
        } else if month < (31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30) * day_dur {
            day = month - (31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31) * day_dur;
            11
        } else if month < (31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30 + 31) * day_dur {
            day = month - (31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30) * day_dur;
            12
        } else {
            0
        };
        let mut hour = day % day_dur;
        day /= day_dur;
        day += 1;
        if leap {
            day += 1;
        }
        let mut minute = hour % hour_dur;
        hour /= hour_dur;
        let mut second = minute % minute_dur;
        minute /= minute_dur;
        let milisec = second % second_dur;
        second /= second_dur;
        format!(
            "[ {}-{}-{} {}:{}:{}.{} ]",
            year, month, day, hour, minute, second, milisec
        )
    }
}

impl Eq for SystemTime {}
impl PartialEq for SystemTime {
    fn eq(&self, other: &Self) -> bool {
        self.unix_time == other.unix_time && self.ns_time == other.ns_time
    }
}

impl Ord for SystemTime {
    fn cmp(&self, other: &Self) -> Ordering {
        match self.unix_time.cmp(&other.unix_time) {
            Ordering::Equal => self.ns_time.cmp(&other.ns_time),
            ord => ord,
        }
    }
}

impl PartialOrd for SystemTime {
    fn partial_cmp(&self, other: &Self) -> Option<Ordering> {
        match self.unix_time.partial_cmp(&other.unix_time) {
            Some(Ordering::Equal) => self.ns_time.partial_cmp(&other.ns_time),
            ord => ord,
        }
    }
}

impl Sub<SystemTime> for SystemTime {
    type Output = Result<Duration, SystemTimeError>;

    fn sub(self, rhs: SystemTime) -> Self::Output {
        if self < rhs {
            let earl = (rhs - self).unwrap();
            Err(SystemTimeError(earl))
        } else {
            let usdiff = self.ns_time as isize - self.ns_time as isize;
            let usdiff = if usdiff >= 0 {
                Duration::from_nanos(usdiff as usize as u64)
            } else {
                Duration::from_nanos(0) - Duration::from_nanos((-usdiff) as usize as u64)
            };
            Ok(Duration::from_millis((self.unix_time - rhs.unix_time) as u64) + usdiff)
        }
    }
}

impl SystemTime {
    pub fn now() -> Self {
        unsafe {
            Self {
                unix_time: system_time_get(),
                ns_time: system_time_ns_get(),
            }
        }
    }

    pub fn duration_since(&self, earlier: &SystemTime) -> Result<Duration, SystemTimeError> {
        *self - *earlier
    }

    pub fn elapsed(&self) -> Result<Duration, SystemTimeError> {
        SystemTime::now() - *self
    }
}
