#ifndef TIME_H
#define TIME_H

#include <types.h>

// 使用UNIX时间戳
usize system_time_get();

// 如果硬件支持更高的计时精度，
// 此函数提供从系统unix时间开始到现在的纳秒为单位的时间
usize system_time_ns_get();

void system_time_increase();

#endif
